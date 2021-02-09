#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <thread>

#include <cstddef>
#include <ctime>

#include <openssl/sha.h>
// #include <grpc/grpc.h>
// #include <grpcpp/security/server_credentials.h>
// #include <grpcpp/server.h>
// #include <grpcpp/server_builder.h>
// #include <grpcpp/server_context.h>

// #include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <vector>

#include "registry.grpc.pb.h"
#include "registry.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using namespace registry;

class PeerMessageWriter;
struct PeerInfo {
    std::string name;
    std::uint64_t last_active;
    PeerMessageWriter *writer_;
    PeerInfo(std::string name, std::uint64_t last_active)
        : name(name)
        , last_active(last_active)
        , writer_(nullptr) {}
};

std::uint64_t GetUTC() {
    std::chrono::time_point currently
        = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    std::chrono::duration millis_since_utc_epoch = currently.time_since_epoch();
    return millis_since_utc_epoch.count();
}

std::string GetAccessToken(::grpc::ServerContext *context) {
    auto it = context->client_metadata().find("Access-Token");
    auto clientId = std::string(it->second.begin(), it->second.end());
    return clientId;
}

#if 0
class RegistryImpl final : public Registry::Service {
public:
    explicit RegistryImpl() {}

    ::grpc::Status Login(::grpc::ServerContext *context,
        const ::registry::Credential *request,
        ::registry::AccessToken *response) {
        std::string name = request->name();
        unsigned char buf[SHA_DIGEST_LENGTH];
        SHA1((unsigned char *)name.data(), name.size(), buf);
        std::string to_string;
        for (std::size_t i = 0; i < sizeof(buf); i++) {
            to_string += (i >> 4) + '0';
            to_string += (i & 0xf) + '0';
        }
        response->set_token(to_string);
        auto peer_info(std::make_shared<PeerInfo>(name, GetUTC()));

        peer_id_map_.emplace(to_string, peer_info);
        peer_name_map_.emplace(name, peer_info);
        return ::grpc::Status::OK;
    }

    ::grpc::Status Ping(::grpc::ServerContext *context,
        const ::registry::Nothing *request, ::registry::Time *response) {
        auto peer(GetPeerInfo(context));
        if (!peer) {
            return ::grpc::Status::CANCELLED;
        }
        auto now(GetUTC());
        peer->last_active = now;
        response->set_utc(now);
        return ::grpc::Status::OK;
    }

    ::grpc::Status ListPeers(::grpc::ServerContext *context,
        const ::registry::Nothing *request, ::registry::PeerList *response) {
        auto peer(GetPeerInfo(context));
        if (!peer) {
            return ::grpc::Status::CANCELLED;
        }
        for (auto const &[_, p] : peer_id_map_) {
            auto new_one(response->add_peers());
            new_one->mutable_id()->set_id(p->name);
        }
        return ::grpc::Status::OK;
    }

    ::grpc::Status SwitchMessages(::grpc::ServerContext *context,
        ::grpc::ServerReaderWriter<::registry::Message, ::registry::Message>
            *stream) {
        auto peer(GetPeerInfo(context));
        if (!peer) {
            return ::grpc::Status::CANCELLED;
        }
        while (true) {
            ::registry::Message msg;
            if (!stream->Read(&msg)) {
                break;
            }
            auto it = peer_name_map_.find(msg.peer().id());
            if (it == peer_name_map_.end()) {
                return ::grpc::Status::CANCELLED;
            }
        }

        return ::grpc::Status::OK;
    }

    ::grpc::Status WaitForPeerConnection(::grpc::ServerContext *context,
        const ::registry::Nothing *request,
        ::grpc::ServerWriter<::registry::Peer> *writer) {}

    ::grpc::Status StartPeerConnection(::grpc::ServerContext *context,
        ::grpc::ServerReaderWriter<::registry::Bytes, ::registry::Bytes>
            *stream) {}

    PeerInfo *GetPeerInfo(::grpc::ServerContext *context) {
        auto it = peer_id_map_.find(GetAccessToken(context));
        if (it == peer_id_map_.end()) {
            return nullptr;
        }
        return it->second.get();
    }

private:
    std::mutex mu_;

    std::map<std::string, std::shared_ptr<PeerInfo>> peer_id_map_;
    std::map<std::string, std::shared_ptr<PeerInfo>> peer_name_map_;
};
#endif

class HandleCall;
std::map<std::string, std::shared_ptr<PeerInfo>> peer_id_map_;
std::map<std::string, std::shared_ptr<PeerInfo>> peer_name_map_;

PeerInfo *GetPeerInfo(::grpc::ServerContext *context) {
    auto it = peer_id_map_.find(GetAccessToken(context));
    if (it == peer_id_map_.end()) {
        return nullptr;
    }
    return it->second.get();
}

class HandleCallBase {
public:
    virtual ~HandleCallBase() {}

    virtual void Proceed() = 0;
};

class HandleCall : public HandleCallBase {
public:
    HandleCall(
        registry::Registry::AsyncService *service, ServerCompletionQueue *q)
        : service_(service)
        , q_(q)
        , status_(CREATE) {}

    virtual ~HandleCall() {}

    void Proceed() override {
        switch (status_) {
            case CREATE:
                status_ = RECEIVED_REQUEST;
                RequestData();
                break;
            case RECEIVED_REQUEST:
                CreateInstance(service_, q_)->Proceed();
                if (HandleRequest()) {
                    status_ = FINALLIZE;
                } else {
                    status_ = PROCESS;
                }
                break;
            case PROCESS:
                if (HandleRequest()) {
                    status_ = FINALLIZE;
                }
                break;
            case FINALLIZE:
                delete this;
            default:
                break;
        }
    }

    virtual void RequestData() = 0;
    virtual bool HandleRequest() = 0;

    virtual HandleCall *CreateInstance(
        registry::Registry::AsyncService *service, ServerCompletionQueue *q)
        = 0;

protected:
    registry::Registry::AsyncService *service_;
    ServerCompletionQueue *q_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, RECEIVED_REQUEST, PROCESS, FINALLIZE };
    CallStatus status_; // The current serving state.
};

class HandleLogin : public HandleCall {
public:
    HandleLogin(
        registry::Registry::AsyncService *service, ServerCompletionQueue *q)
        : HandleCall(service, q)
        , response_writer(&ctx_) {}

    void RequestData() override {
        service_->RequestLogin(&ctx_, &request, &response_writer, q_, q_, this);
    }

    bool HandleRequest() override {
        std::string name = request.name();
        unsigned char buf[SHA_DIGEST_LENGTH];
        SHA1((unsigned char *)name.data(), name.size(), buf);
        std::stringstream ss;
        ss << std::hex;
        for (std::size_t i = 0; i < sizeof(buf); i++) {
            ss << (unsigned int)buf[i];
        }
        std::string to_string = ss.str();
        response.set_token(to_string);
        auto peer_info(std::make_shared<PeerInfo>(name, GetUTC()));
        peer_id_map_.emplace(to_string, peer_info);
        peer_name_map_.emplace(name, peer_info);
        response_writer.Finish(response, ::grpc::Status::OK, this);
        return true;
    }

    HandleCall *CreateInstance(registry::Registry::AsyncService *service,
        ServerCompletionQueue *q) override {
        return new HandleLogin(service, q);
    }

    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ::grpc::ServerContext ctx_;
    ::registry::Credential request;
    ::registry::AccessToken response;
    ::grpc::ServerAsyncResponseWriter<::registry::AccessToken> response_writer;
};

class HandlePing : public HandleCall {
public:
    HandlePing(
        registry::Registry::AsyncService *service, ServerCompletionQueue *q)
        : HandleCall(service, q)
        , response_writer(&ctx_) {}

    void RequestData() override {
        service_->RequestPing(&ctx_, &request, &response_writer, q_, q_, this);
    }

    bool HandleRequest() override {
        auto peer(GetPeerInfo(&ctx_));
        if (!peer) {
            response_writer.FinishWithError(::grpc::Status::CANCELLED, this);
            return true;
        }
        auto now(GetUTC());
        peer->last_active = now;
        response.set_utc(now);
        response_writer.Finish(response, ::grpc::Status::OK, this);
        return true;
    }

    HandleCall *CreateInstance(registry::Registry::AsyncService *service,
        ServerCompletionQueue *q) override {
        return new HandlePing(service, q);
    }

    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ::grpc::ServerContext ctx_;
    ::registry::Nothing request;
    ::registry::Time response;
    ::grpc::ServerAsyncResponseWriter<::registry::Time> response_writer;
};

class ListPeers : public HandleCall {
public:
    ListPeers(
        registry::Registry::AsyncService *service, ServerCompletionQueue *q)
        : HandleCall(service, q)
        , response_writer(&ctx_) {}

    void RequestData() override {
        service_->RequestListPeers(
            &ctx_, &request, &response_writer, q_, q_, this);
    }

    bool HandleRequest() override {
        auto peer(GetPeerInfo(&ctx_));
        if (!peer) {
            response_writer.FinishWithError(::grpc::Status::CANCELLED, this);
            return true;
        }
        for (auto const &[_, p] : peer_id_map_) {
            auto new_one(response.add_peers());
            new_one->mutable_id()->set_id(p->name);
        }
        response_writer.Finish(response, ::grpc::Status::OK, this);
        return true;
    }

    HandleCall *CreateInstance(registry::Registry::AsyncService *service,
        ServerCompletionQueue *q) override {
        return new ListPeers(service, q);
    }

    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ::grpc::ServerContext ctx_;
    ::registry::Nothing request;
    ::registry::PeerList response;
    ::grpc::ServerAsyncResponseWriter<::registry::PeerList> response_writer;
};

class MessageSwitchContext {
public:
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ::grpc::ServerContext ctx_;
    ::grpc::ServerAsyncReaderWriter<::registry::Message, ::registry::Message>
        stream;
    PeerInfo *peer;

    MessageSwitchContext()
        : stream(&ctx_)
        , peer(nullptr) {}
};

class PeerMessageWriter : public HandleCallBase {
public:
    PeerMessageWriter(std::shared_ptr<MessageSwitchContext> ptr)
        : ptr_(ptr)
        , status_(IDLE) {}

    void Proceed() override {
        if (ptr_->ctx_.IsCancelled()) {
            ptr_->peer->writer_ = nullptr;
            delete this;
            return;
        }
        switch (status_) {
            case IDLE:
                break;
            case RUNNING:
                to_write_.pop_front();
                if (!to_write_.empty()) {
                    ptr_->stream.Write(to_write_.front(), this);
                } else {
                    status_ = IDLE;
                }
            default:
                break;
        }
    }

    void Write(::registry::Message msg) {
        to_write_.emplace_back(msg);
        switch (status_) {
            case IDLE:
                ptr_->stream.Write(to_write_.front(), this);
                status_ = RUNNING;
                break;
            case RUNNING:
            default:
                break;
        }
    }

    enum { IDLE = 0, RUNNING = 1 } status_;
    std::shared_ptr<MessageSwitchContext> ptr_;
    std::deque<::registry::Message> to_write_;
};

class HandleSwitchMessages : public HandleCall {
public:
    class WriteBack : public HandleCallBase {
    public:
        WriteBack(HandleSwitchMessages *p)
            : p_(p) {}
        void Proceed() {}

        void WriteMessage(::registry::Message *msg) {}

        HandleSwitchMessages *p_;
    };

    HandleSwitchMessages(
        registry::Registry::AsyncService *service, ServerCompletionQueue *q)
        : HandleCall(service, q)
        , ctx_(new MessageSwitchContext) {}

    void RequestData() override {
        service_->RequestSwitchMessages(
            &ctx_->ctx_, &ctx_->stream, q_, q_, this);
    }

    bool HandleRequest() override {
        if (!peer) {
            peer = GetPeerInfo(&ctx_->ctx_);
            if (!peer) {
                ctx_->stream.Finish(::grpc::Status::CANCELLED, this);
                return true;
            }
            ctx_->peer = peer;
            ctx_->peer->writer_ = new PeerMessageWriter(ctx_);
            // prepare read
            ctx_->stream.Read(&msg, this);
            return false;
        }
        if (ctx_->ctx_.IsCancelled()) {
            return true;
        }
        // handle message
        auto dest(msg.peer().id());
        auto it = peer_name_map_.find(dest);
        if (it != peer_name_map_.end()) {
            if (it->second->writer_) {
                *(msg.mutable_peer()->mutable_id()) = peer->name;
                it->second->writer_->Write(msg);
            }
        }
        ctx_->stream.Read(&msg, this);
        return false;
    }

    HandleCall *CreateInstance(registry::Registry::AsyncService *service,
        ServerCompletionQueue *q) override {
        return new HandleSwitchMessages(service, q);
    }

    std::shared_ptr<MessageSwitchContext> ctx_;
    ::registry::Message msg;

    PeerInfo *peer;
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    // RegistryImpl service;
    registry::Registry::AsyncService service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<ServerCompletionQueue> cq(builder.AddCompletionQueue());
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    // server->Wait();

    (new HandleLogin(&service, cq.get()))->Proceed();

    void *tag; // uniquely identifies a request.
    bool ok;
    while (true) {
        // Block waiting to read the next event from the completion queue. The
        // event is uniquely identified by its tag, which in this case is the
        // memory address of a CallData instance.
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or cq_ is shutting down.
        GPR_ASSERT(cq->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<HandleCallBase *>(tag)->Proceed();
    }
}

int main(int argc, char **argv) {
    RunServer();
    return 0;
}