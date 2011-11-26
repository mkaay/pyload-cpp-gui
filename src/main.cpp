#include <QApplication>
#include <QSettings>

#include "application.h"

#include "thrift/client.h"

#include "ui/mainwindow/mainwindow.h"
#include "ui/connectionmanager/connectionmanager.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    QCoreApplication::setOrganizationName("pyLoad");
    QCoreApplication::setOrganizationDomain("pyload.org");
    QCoreApplication::setApplicationName("pyLoad GUI");

    QSettings settings;

    MainWindow *w = new MainWindow();
    ConnectionManager *cm = new ConnectionManager();

    w->connect(w, SIGNAL(connectionManager()), cm, SLOT(show()));
    w->connect(w, SIGNAL(connectionManager()), w, SLOT(hide()));
    cm->setMainWindow(w);

    if (settings.value("firststart", true).toBool()) {
        settings.setValue("firststart", false);

        cm->show();
    } else {
        cm->doConnect();
    }

    return a.exec();
}
