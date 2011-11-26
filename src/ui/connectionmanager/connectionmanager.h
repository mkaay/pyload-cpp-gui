#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QWidget>
#include <QSettings>
#include <QKeyEvent>

#include "../../thrift/client.h"

class MainWindow;

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

    void on_actionReturnPressed_triggered();

private:
    Ui::ConnectionManager *ui;
    MainWindow *main;
    ThriftClient *client;

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
};

#endif // CONNECTIONMANAGER_H
