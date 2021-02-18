#pragma once

#include <QObject>
#include <QThread>

#include "registry.grpc.pb.h"
#include "registry.pb.h"
#include <grpcpp/grpcpp.h>

struct PeerId {
    PeerId() = default;
    ~PeerId() = default;
    PeerId(const PeerId &) = default;
    PeerId &operator=(const PeerId &) = default;
    QString id;
};
struct PeerList {
    PeerList() = default;
    ~PeerList() = default;
    PeerList(const PeerList &) = default;
    PeerList &operator=(const PeerList &) = default;

    QVector<PeerId> peers;
};

Q_DECLARE_METATYPE(PeerId);
Q_DECLARE_METATYPE(PeerList);

class GRPCThreads : public QThread {
    Q_OBJECT
public:
    GRPCThreads(QObject *parent = nullptr)
        : QThread(parent)
        , stub_(::registry::Registry::NewStub(::grpc::CreateChannel(
              "localhost:50051", grpc::InsecureChannelCredentials()))) {}

    void run();

public slots:
    void Login(QString const &cred);
    void ListPeers(QString const &cred);
    // void Ping();

signals:
    void OnLogin(QString const &id);
    void OnListPeers(PeerList const &list);
    // void PingCompleted()

protected:
    ::grpc::CompletionQueue cq_;
    ::grpc::Status status_;

    std::unique_ptr<::grpc::ClientAsyncResponseReader<::registry::Credential>>
        login_;

    std::unique_ptr<::registry::Registry::Stub> stub_;
};