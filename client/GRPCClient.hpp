#pragma once

#include <QObject>
#include <QThread>

#include "registry.grpc.pb.h"
#include "registry.pb.h"
#include <grpcpp/grpcpp.h>

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
    // void Ping();

signals:
    void LoginCompleted(QString const &id);
    // void PingCompleted()

protected:
    ::grpc::CompletionQueue cq_;
    ::grpc::Status status_;

    std::unique_ptr<::grpc::ClientAsyncResponseReader<::registry::Credential>>
        login_;

    std::unique_ptr<::registry::Registry::Stub> stub_;
};