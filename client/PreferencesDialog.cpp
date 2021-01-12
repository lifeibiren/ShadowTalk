#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include <qnamespace.h>
#include <qtablewidget.h>

PreferencesDialog::PreferencesDialog(
    const std::map<QString, QString> &conf, QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::PreferencesDialog)
    , conf_(conf) {
    ui_->setupUi(this);

    ui_->tableWidget->setRowCount(conf.size());
    ui_->tableWidget->setColumnCount(2);

    int i = 0;
    for (auto const &[k, v] : conf) {
        QTableWidgetItem *k_item = new QTableWidgetItem(k);
        k_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_->tableWidget->setItem(i, 0, k_item);

        ui_->tableWidget->setItem(i, 1, new QTableWidgetItem(v));
        ++i;
    }
}

PreferencesDialog::~PreferencesDialog() {
    delete ui_;
}

void PreferencesDialog::accept() {
    conf_.clear();
    for (int i = 0; i < ui_->tableWidget->rowCount(); ++i) {
        QTableWidgetItem *k = ui_->tableWidget->item(i, 0);
        QTableWidgetItem *v = ui_->tableWidget->item(i, 1);
        conf_.emplace(k->text(), v->text());
    }

    QDialog::accept();
}

std::map<QString, QString> const &PreferencesDialog::GetConf() const noexcept {
    return conf_;
}