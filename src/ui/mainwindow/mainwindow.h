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

#include "../../thrift/client.h"
#include "../../thrift/interface/Pyload.h"

#include "../downloadsmodel.h"

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

signals:
    void proxyReady();
    void connectionManager();

private:
    Ui::MainWindow *ui;
    PyloadClient *proxy;
    QLabel downloadStatus;

    uint logOffset;
    std::vector<ConfigSection> coresections;
    std::vector<ConfigSection> pluginsections;

    void readSettings();
    void writeSettings();
    void updateLog();
    void updateServerStatus();
    void initDownloads();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
