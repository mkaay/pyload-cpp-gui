#include "connectionmanager.h"
#include "ui_connectionmanager.h"

#include "../mainwindow/mainwindow.h"

ConnectionManager::ConnectionManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionManager)
{
    ui->setupUi(this);

    QSettings settings;
    ui->hostLineEdit->setText(settings.value("connection/host").toString());
    if (settings.value("connection/port").toInt() > 0) {
        ui->portSpinBox->setValue(settings.value("connection/port").toInt());
    } else {
        ui->portSpinBox->setValue(7227);
    }
    ui->userLineEdit->setText(settings.value("connection/user").toString());
    ui->passwordLineEdit->setText(settings.value("connection/password").toString());

    ui->portSpinBox->installEventFilter(this);

    connect(this, SIGNAL(_doConnect()), this, SLOT(_connect()), Qt::QueuedConnection);
}

ConnectionManager::~ConnectionManager()
{
    delete ui;
}

void ConnectionManager::setMainWindow(MainWindow *w)
{
    main = w;
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

    emit _doConnect();
}

void ConnectionManager::doConnect()
{
    emit _doConnect();
}

void ConnectionManager::_connect()
{
    client = new ThriftClient();

    connect(client, SIGNAL(connected(bool)), this, SLOT(connected(bool)));

    QSettings settings;
    client->doConnect(settings.value("connection/host").toString(),
                settings.value("connection/port").toInt(),
                settings.value("connection/user").toString(),
                settings.value("connection/password").toString());
}

void ConnectionManager::connected(bool ok)
{
    if (ok) {
        disconnect(client, SIGNAL(connected(bool)), this, SLOT(connected(bool)));
        hide();
        main->show();
        main->setClient(client);
    } else {
        if (!isVisible()) {
            show();
        }
    }

    ui->hostLineEdit->setEnabled(true);
    ui->portSpinBox->setEnabled(true);
    ui->userLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->connectButton->setEnabled(true);
}

bool ConnectionManager::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == ui->portSpinBox) {
        if (ev->type() == QEvent::KeyPress) {
            QKeyEvent *event = static_cast<QKeyEvent *>(ev);
            if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
                emit on_connectButton_clicked();
                return true;
            }
        }
    }

    return false;
}

void ConnectionManager::on_actionReturnPressed_triggered()
{
    emit on_connectButton_clicked();
}

void ConnectionManager::closeConnection()
{
    client->disconnect();
}
