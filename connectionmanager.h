#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QWidget>
#include <QSettings>

#include "thriftclient.h"
#include "mainwindow.h"

namespace Ui {
    class ConnectionManager;
}

class ConnectionManager : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionManager(QWidget *parent = 0);
    ~ConnectionManager();

private slots:
    void on_connectButton_clicked();

private:
    Ui::ConnectionManager *ui;
};

#endif // CONNECTIONMANAGER_H
