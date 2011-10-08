#include <QApplication>
#include <QSettings>

#include "ui/mainwindow/mainwindow.h"
#include "ui/connectionmanager/connectionmanager.h"
#include "thrift/client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("pyLoad");
    QCoreApplication::setOrganizationDomain("pyload.org");
    QCoreApplication::setApplicationName("pyLoad GUI");

    QSettings settings;

    MainWindow *w = new MainWindow();
    ConnectionManager *cm = new ConnectionManager();

    //MainWindow w();
    //ConnectionManager cm();
    cm->setMainWindow(w);

    if (settings.value("firststart", true).toBool()) {
        settings.setValue("firststart", false);

        cm->show();
    } else {
        cm->doConnect();
    }

    return a.exec();
}
