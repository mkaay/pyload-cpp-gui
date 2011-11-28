#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <QThread>
#include <QUuid>
#include <QSettings>
#include <QTimer>
#include <QSharedPointer>

#include "thrift/client.h"
#include "thrift/interface/Pyload.h"

#include "file.h"

class EventLoop : public QThread
{
    Q_OBJECT
public:
    explicit EventLoop(QObject *parent = 0);
    void init();

signals:
    void addFile(Pyload::FileData &f);
    void addPackage(Pyload::PackageData &p);
    void addPackage(Pyload::PackageInfo &p);
    void removeFile(int id);
    void removePackage(int id);
    void updatePackage(Pyload::PackageData &p);
    void updateFile(Pyload::FileData &f);
    void updateDownloadStatus(Pyload::DownloadInfo &i);
    void newSpeed(int speed);

public slots:

private slots:
    void connectionEstablished(bool ok);
    void updateTrigger();

private:
    ThriftClient *client;
    PyloadClient *proxy;

    QUuid uuid;
    QSettings settings;
    QTimer timer;
    QMutex mutex;

    bool initQueue;
    bool initCollector;

    void reloadQueue();
    void reloadCollector();
    void insertPackage(int id);
    void insertFile(int id);

    void updateFiles(int id);

protected:
    void run();
};

#endif // EVENTLOOP_H
