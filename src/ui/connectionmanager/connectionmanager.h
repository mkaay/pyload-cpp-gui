#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QWidget>
#include <QSettings>

#include "../../thrift/client.h"
#include "../mainwindow/mainwindow.h"

namespace Ui {
    class ConnectionManager;
}

class ConnectionManager : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionManager(QWidget *parent = 0);
    ~ConnectionManager();

    void setMainWindow(MainWindow *w);
    void doConnect();

signals:
    void _doConnect();

private slots:
    void on_connectButton_clicked();
    void connected(bool ok);
    void _connect();

private:
    Ui::ConnectionManager *ui;
    MainWindow *main;
    ThriftClient *client;
};

#endif // CONNECTIONMANAGER_H
