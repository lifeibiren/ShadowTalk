#include "GRPCClient.hpp"
#include <QDebug>

#include <grpc/grpc.h>

int Reg = []() {
    int id = qRegisterMetaType<PeerList>();
    return 0;
}();

class RPCCall : public RPCEvent {
public:
    RPCCall(::registry::Registry::Stub *stub, ::grpc::CompletionQueue *cq)
        : stub_(stub)
        , cq_(cq) {}

    ::registry::Registry::Stub *stub_;
    ::grpc::CompletionQueue *cq_;
    ::grpc::Status status_;
};

class LoginCall : public RPCCall {
public:
    LoginCall(::registry::Registry::Stub *stub, ::grpc::CompletionQueue *cq,
        QString const &id, std::function<void(QString const &)> callback)
        : RPCCall(stub, cq)
        , cb_(callback)
        , state_(INIT) {
        request_.set_name(id.toStdString());
        Proceed(true);
    }

    void Proceed(bool ok) override {
        switch (state_) {
            case INIT:
                reader_ = stub_->AsyncLogin(&context_, request_, cq_);
                reader_->Finish(&response_, &status_, this);
                state_ = FIN;
                break;
            case FIN:
                if (status_.ok()) {
                    cb_(QString::fromStdString(response_.token()));
                } else {
                    qDebug("Login failed");
                }
                delete this;
                break;
            default:
                break;
        }
    }

    enum { INIT, FIN } state_;
    ::registry::Credential request_;
    ::registry::AccessToken response_;

    ::grpc::ClientContext context_;
    std::unique_ptr<::grpc::ClientAsyncResponseReader<::registry::AccessToken>>
        reader_;

    std::function<void(QString const &)> cb_;
};

class ListPeerCall : public RPCCall {
public:
    ListPeerCall(::registry::Registry::Stub *stub, ::grpc::CompletionQueue *cq,
        QString const &token, std::function<void(PeerList const &)> callback)
        : RPCCall(stub, cq)
        , token_(token.toStdString())
        , cb_(callback)
        , state_(INIT) {
        context_.AddMetadata("access-token", token_);
        Proceed(true);
    }

    void Proceed(bool ok) override {
        switch (state_) {
            case INIT:
                reader_ = stub_->AsyncListPeers(&context_, request_, cq_);
                reader_->Finish(&response_, &status_, this);
                state_ = FIN;
                break;
            case FIN:
                if (status_.ok()) {
                    PeerList r;
                    for (auto const &p : response_.peers()) {
                        PeerId id;
                        id.id = QString::fromStdString(p.id().id());
                        r.peers.push_back(id);
                    }
                    cb_(r);
                } else {
                    qDebug("ListPeer failed");
                }
                delete this;
                break;
            default:
                break;
        }
    }

    enum { INIT, FIN } state_;
    std::string token_;
    ::registry::Nothing request_;
    ::registry::PeerList response_;

    ::grpc::ClientContext context_;
    std::unique_ptr<::grpc::ClientAsyncResponseReader<::registry::PeerList>>
        reader_;

    std::function<void(PeerList const &)> cb_;
};

struct MessageSender : public RPCCall {
    MessageSender(::registry::Registry::Stub *stub, ::grpc::CompletionQueue *cq,
        std::shared_ptr<Context> const &ptr)
        : RPCCall(stub, cq)
        , ctx_(ptr)
        , state_(INIT) {
        Proceed(true);
    }

    void Proceed(bool ok) override {
        switch (state_) {
            case INIT:
                state_ = RUNNING;
            case RUNNING: {
                if (!ok) {
                    qDebug("Message sender exits");
                    delete this;
                    return;
                }
                auto m_opt = ctx_->Dequeue();
                if (!m_opt.has_value()) {
                    qDebug("Message sender sleeps");
                    break;
                }
                qDebug("Message sender sends data");
                m_ = m_opt.value();
                to_send_.mutable_peer()->set_id(m_.peer.id.toStdString());
                to_send_.set_type(
                    ::registry::Message_MessageType::Message_MessageType_TEXT);
                to_send_.set_content(m_.bytes.data(), m_.bytes.size());
                ctx_->rw_->Write(to_send_, this);
                break;
            }
            default:
                break;
        }
    }

    std::shared_ptr<Context> ctx_;
    enum { INIT, RUNNING } state_;
    ::registry::Message to_send_;
    Message m_;
};

class SwitchMessagesCall : public RPCCall {
public:
    SwitchMessagesCall(::registry::Registry::Stub *stub,
        ::grpc::CompletionQueue *cq, std::shared_ptr<Context> ctx,
        std::function<void(Message const &)> callback)
        : RPCCall(stub, cq)
        , ctx_(ctx)
        , cb_(callback)
        , state_(INIT) {
        Proceed(true);
    }

    void Proceed(bool ok) override {
        switch (state_) {
            case INIT:
                ctx_->rw_
                    = stub_->AsyncSwitchMessages(&ctx_->context_, cq_, this);
                state_ = READY;
                break;
            case READY:
                if (!ok) {
                    goto cleanup;
                }
                qDebug("Ready to receive messages");
                ctx_->sender_ = new MessageSender(stub_, cq_, ctx_);
                ctx_->rw_->Read(&received_, this);
                state_ = READING;
                break;
            case READING: {
                if (!ok) {
                    goto cleanup;
                }

                Message msg;
                msg.peer.id.fromStdString(received_.peer().id());
                msg.type = Message::TEXT;
                msg.bytes.resize(received_.content().size());
                memcpy(msg.bytes.data(), received_.content().data(),
                    received_.content().size());

                // auto bytes(
                //     QString::fromRawData((QChar *)received_.content().data(),
                //         received_.content().size())
                //         .toLatin1());
                qDebug("Received messages: %*.s",
                    (int)received_.content().size(),
                    received_.content().data());
                cb_(msg);
                received_.Clear();
                ctx_->rw_->Read(&received_, this);
                break;
            }
            default:
                break;
        }
        return;

    cleanup:
        delete this;
    }

    std::shared_ptr<Context> ctx_;
    enum { INIT, READY, READING, FIN } state_;
    std::string token_;
    ::registry::Message received_;

    std::function<void(Message const &)> cb_;
};

void GRPCThreads::run() {
    qDebug("Start GRPC Client");
    // start Login call
    Login(cred_);

    void *tag; // uniquely identifies a request.
    bool ok;
    forever {
        // Block waiting to read the next event from the completion queue. The
        // event is uniquely identified by its tag, which in this case is the
        // memory address of a CallData instance.
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or cq_ is shutting down.
        GPR_ASSERT(cq_.Next(&tag, &ok));
        HandleEvent(static_cast<RPCEvent *>(tag), ok);
    }
}

void GRPCThreads::Login(QString const &cred) {
    new LoginCall(stub_.get(), &cq_, cred, [this](QString const &token) {
        token_ = token;
        startMessageTransmitting();
        emit OnLogin(token);
    });
}

void GRPCThreads::ListPeers() {
    new ListPeerCall(stub_.get(), &cq_, token_,
        [this](PeerList const &list) { emit OnListPeers(list); });
}

void GRPCThreads::startMessageTransmitting() {
    ctx_ = std::make_shared<Context>(token_.toStdString());
    new SwitchMessagesCall(stub_.get(), &cq_, ctx_,
        [this](Message const &msg) { emit OnMessageReceived(msg); });
}

void GRPCThreads::SendMessage(Message const &msg) {
    ctx_->Enqueue(msg);
}

Context::Context(std::string const &token)
    : running_(false)
    , sender_(nullptr) {
    context_.AddMetadata("access-token", token);
}

void Context::Enqueue(Message const &m) {
    qDebug("Enqueue message");
    msg_q_.emplace_back(m);
    if (!running_ && sender_) {
        qDebug("Trigger sender");
        // trigger write
        running_ = true;
        sender_->Proceed(true);
    }
}

std::optional<Message> Context::Dequeue() {
    if (msg_q_.empty()) {
        // returning nullopt would stop writing
        running_ = false;
        return std::nullopt;
    }
    Message m = msg_q_.front();
    msg_q_.pop_front();
    return m;
}

void GRPCThreads::HandleEvent(RPCEvent *call, bool ok) {
    std::lock_guard g(m_);
#if 0
    switch (state_) {
        default:
            break;
        case INIT:
            return;
        case LOGINING:
            return;
        case READY:
            return;
    }
    call->PreProcess(ok);
    switch (call->Kind()) {
        default:
            break;
        case StartLogin:
        case LoginComplete:
        case StartListPeers:
        case ListPeersComplete:
        case SendMessages:
        case SendMessagesComplete:
            break;
    }
#endif
    call->Proceed(ok);
    // call->PostProcess(ok);
    // delete call;
}
