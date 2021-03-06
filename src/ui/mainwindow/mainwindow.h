#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QTimer>
#include <QLabel>
#include <QClipboard>
#include <QRegExp>
#include <QUuid>
#include <QMenu>
#include <QFileDialog>
#include <QWidgetAction>

#include "../../thrift/client.h"
#include "../../thrift/interface/Pyload.h"

#include "../downloadsmodel.h"
#include "../progressdelegate.h"
#include "../statusdelegate.h"
#include "../plugindelegate.h"
#include "../speedwidget.h"

class EventLoop;
class DownloadList;

using namespace Pyload;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setClient(ThriftClient *client);

private slots:
    void startLoop();
    void settingsSectionChanged(QTreeWidgetItem *item, int column);
    void updateTrigger();
    void clipboardChanged();

    void on_actionConnectionManager_triggered();
    void on_actionExit_triggered();
    void on_actionSettings_triggered();
    void on_actionWiki_triggered();
    void on_actionForum_triggered();
    void on_actionPause_triggered();

    void on_actionAdd_triggered();
    void addContainer();

signals:
    void proxyReady();
    void connectionManager();
    void closeConnection();

private:
    Ui::MainWindow *ui;
    PyloadClient *proxy;
    QMenu addMenu;
    DownloadsModel model;
    QSharedPointer<EventLoop> eventloop;
    SpeedWidget *speedwidget;

    uint logOffset;
    std::vector<ConfigSection> coresections;
    std::vector<ConfigSection> pluginsections;

    void readSettings();
    void writeSettings();
    void updateLog();
    void updateServerStatus();
    void initQueue();
    void initCollector();
    std::vector<PackageData> getPackages(Destination::type destination);
    PackageData getPackageData(PackageID id);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
