#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <qdebug.h>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(
        const std::map<QString, QString> &conf, QWidget *parent = nullptr);
    ~PreferencesDialog();

    void accept();
    std::map<QString, QString> const &GetConf() const noexcept;

public slots:
    void delete_dialog() { delete this; }

private:
    Ui::PreferencesDialog *ui_;
    std::map<QString, QString> conf_;
};

#endif // PREFERENCES_H
