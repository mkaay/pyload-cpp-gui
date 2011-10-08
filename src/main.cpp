#include <QtGui/QApplication>
#include "QSettings"

#include "mainwindow.h"
#include "connectionmanager.h"
#include "thriftclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("pyLoad");
    QCoreApplication::setOrganizationDomain("pyload.org");
    QCoreApplication::setApplicationName("pyLoad GUI");

    QSettings settings;

    bool connected = false;

    ConnectionManager *cm;
    MainWindow *w;

    if (settings.value("firststart", true).toBool())
    {
        settings.setValue("firststart", false);

        cm = new ConnectionManager();
        cm->show();
    }
    else
    {
        ThriftClient *tc = new ThriftClient();
        connected = tc->connect(settings.value("connection/host", "127.0.0.1").toString(),
                   settings.value("connection/port", "7227").toInt(),
                   settings.value("connection/user", "User").toString(),
                   settings.value("connection/password", "password").toString());
        if (connected)
        {
            w = new MainWindow();
            w->setClient(tc);
            w->show();
        }
        else
        {
            cm = new ConnectionManager();
            cm->show();
        }
    }

    return a.exec();
}
