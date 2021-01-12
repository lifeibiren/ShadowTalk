#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QString>

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

private:
    void loadConf(const QJsonDocument &doc);

private:
    std::unique_ptr<Ui::MainWindow> ui_;
    PreferencesDialog *preferences_dialog_;
    QFile *config_file_;

    std::map<QString, QString> conf_;
};

#endif // MAINWINDOW_H
