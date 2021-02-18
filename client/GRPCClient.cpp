#include "GRPCClient.hpp"
#include <QDebug>

class RPCCall {
public:
    RPCCall(::registry::Registry::Stub *stub, ::grpc::CompletionQueue *cq)
        : stub_(stub)
        , cq_(cq) {}
    virtual ~RPCCall() {}
    virtual void Proceed() = 0;

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
        Proceed();
    }

    void Proceed() {
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
        Proceed();
    }

    void Proceed() {
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

void GRPCThreads::run() {

    void *tag; // uniquely identifies a request.
    bool ok;
    forever {
        // Block waiting to read the next event from the completion queue. The
        // event is uniquely identified by its tag, which in this case is the
        // memory address of a CallData instance.
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or cq_ is shutting down.
        GPR_ASSERT(cq_.Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<RPCCall *>(tag)->Proceed();
    }
}

void GRPCThreads::Login(QString const &cred) {
    new LoginCall(stub_.get(), &cq_, cred,
        [this](QString const &token) { emit OnLogin(token); });
}

void GRPCThreads::ListPeers(QString const &token) {
    new ListPeerCall(stub_.get(), &cq_, token,
        [this](PeerList const &list) { emit OnListPeers(list); });
}
