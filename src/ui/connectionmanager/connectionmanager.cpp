#include "connectionmanager.h"
#include "ui_connectionmanager.h"

ConnectionManager::ConnectionManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionManager)
{
    ui->setupUi(this);

    QSettings settings;
    ui->hostLineEdit->setText(settings.value("connection/host").toString());
    ui->portSpinBox->setValue(settings.value("connection/port").toInt());
    ui->userLineEdit->setText(settings.value("connection/user").toString());
    ui->passwordLineEdit->setText(settings.value("connection/password").toString());
}

ConnectionManager::~ConnectionManager()
{
    delete ui;
}

void ConnectionManager::on_connectButton_clicked()
{
    ui->hostLineEdit->setDisabled(true);
    ui->portSpinBox->setDisabled(true);
    ui->userLineEdit->setDisabled(true);
    ui->passwordLineEdit->setDisabled(true);
    ui->connectButton->setDisabled(true);

    QSettings settings;
    settings.setValue("connection/host", ui->hostLineEdit->text());
    settings.setValue("connection/port", ui->portSpinBox->value());
    settings.setValue("connection/user", ui->userLineEdit->text());
    settings.setValue("connection/password", ui->passwordLineEdit->text());

    ThriftClient *tc = new ThriftClient();
    bool connected = tc->connect(settings.value("connection/host").toString(),
               settings.value("connection/port").toInt(),
               settings.value("connection/user").toString(),
               settings.value("connection/password").toString());

    hide();
    if (connected)
    {
        MainWindow *w = new MainWindow();
        w->setClient(tc);
        w->show();

        deleteLater();
    }
    else
    {
        ui->hostLineEdit->setEnabled(true);
        ui->portSpinBox->setEnabled(true);
        ui->userLineEdit->setEnabled(true);
        ui->passwordLineEdit->setEnabled(true);
        ui->connectButton->setEnabled(true);
    }
}
