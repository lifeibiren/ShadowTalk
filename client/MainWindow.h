#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QString>

#include "GRPCClient.hpp"

namespace Ui {
class MainWindow;
}

class PreferencesDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showPreferences();
    void saveConf();
    void onLogin(QString const &token);
    void onListPeers(PeerList const &list);

private:
    void loadConf(const QJsonDocument &doc);
    void start(QString const &host);
    void stop();

private:
    std::unique_ptr<Ui::MainWindow> ui_;
    PreferencesDialog *preferences_dialog_;
    QFile *config_file_;

    std::map<QString, QString> conf_;

    std::unique_ptr<GRPCThreads> thread_;
};

#endif // MAINWINDOW_H
