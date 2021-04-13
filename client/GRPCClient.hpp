#pragma once

#include <QObject>
#include <QThread>

#include <deque>
#include <mutex>

#include "registry.grpc.pb.h"
#include "registry.pb.h"
#include <grpcpp/grpcpp.h>

struct PeerId {
    QString id;
};

struct PeerList {
    QVector<PeerId> peers;
};

struct Message {
    PeerId peer;
    enum MessageType { TEXT = 0 };
    MessageType type;
    QVector<char> bytes;
};

struct MessageSender;

struct Context {
    ::grpc::ClientContext context_;
    std::deque<Message> msg_q_;
    bool running_;

    std::unique_ptr<::grpc::ClientAsyncReaderWriter<::registry::Message,
        ::registry::Message>>
        rw_;

    MessageSender *sender_;

    Context(std::string const &token);

    // thread-safe
    void Enqueue(Message const &);
    std::optional<Message> Dequeue();
};

Q_DECLARE_METATYPE(PeerList)
Q_DECLARE_METATYPE(Message)

enum RPCEventEnum {
    StartLogin,
    LoginComplete,
    StartListPeers,
    ListPeersComplete,
    SendMessages,
    SendMessagesComplete
};

class RPCEvent {
public:
    // virtual RPCEventEnum Kind() = 0;
    virtual ~RPCEvent() {}
    virtual void Proceed(bool ok) {}
    virtual void PreProcess(bool ok) {}
    virtual void PostProcess(bool ok) {}
};

class GRPCThreads : public QThread {
    Q_OBJECT
public:
    GRPCThreads(
        QString const &server, QString const &cred, QObject *parent = nullptr)
        : QThread(parent)
        , cred_(cred)
        , stub_(::registry::Registry::NewStub(::grpc::CreateChannel(
              server.toStdString(), grpc::InsecureChannelCredentials()))) {}

    void run();

    void HandleEvent(RPCEvent *call, bool ok);

public slots:
    void Login(QString const &cred);
    void ListPeers();
    void SendMessage(Message const &msg);
    // void Ping();

signals:
    void OnLogin(QString const &token);
    void OnListPeers(PeerList const &list);
    void OnMessageReceived(Message const &msg);
    // void PingCompleted()

protected:
    void startMessageTransmitting();

    std::mutex m_;

    enum { INIT, LOGINING, READY } state_;

    QString cred_;
    QString token_;

    ::grpc::CompletionQueue cq_;
    ::grpc::Status status_;

    std::unique_ptr<::grpc::ClientAsyncResponseReader<::registry::Credential>>
        login_;

    std::unique_ptr<::registry::Registry::Stub> stub_;

    std::shared_ptr<Context> ctx_;
};