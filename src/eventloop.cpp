#include "eventloop.h"
#include <QDebug>

EventLoop::EventLoop(QObject *parent) : QThread(parent)
{
    moveToThread(this);
    uuid = QUuid::createUuid();
}

void EventLoop::init()
{
    client = new ThriftClient();

    connect(client, SIGNAL(connected(bool)), this, SLOT(connectionEstablished(bool)));

    client->doConnect(settings.value("connection/host").toString(),
                settings.value("connection/port").toInt(),
                settings.value("connection/user").toString(),
                settings.value("connection/password").toString());
}

void EventLoop::connectionEstablished(bool ok)
{
    if (ok) {
        qDebug() << "connection okay" << currentThreadId();
        proxy = client->getProxy();
        start();
    } else {
        qDebug() << "eventloop connection failed!";
    }
}

void EventLoop::run()
{
    qDebug() << "starting eventloop thread" << currentThreadId();

    timer.connect(&timer, SIGNAL(timeout()), this, SLOT(updateTrigger()), Qt::QueuedConnection);
    timer.setSingleShot(true);
    timer.setInterval(0);
    timer.start();

    initQueue = false;
    initCollector = false;

    exec();
}

void EventLoop::updateTrigger()
{
    std::vector<Event> events;

    proxy->getEvents(events, uuid.toString().toStdString());

    Event event;
    foreach (event, events) {
        qDebug() << QString::fromStdString(event.event) << QString::number(event.destination);
        if (event.event == "reload") {
            qDebug() << "invoking reload" << currentThreadId();
            if (event.destination == Destination::Queue) {
                if (!initQueue) {
                    initQueue = true;
                    reloadQueue();
                } else {
                    qDebug() << "ignoring queue reload";
                }
            } else {
                if (!initCollector) {
                    initCollector = true;
                    reloadCollector();
                } else {
                    qDebug() << "ignoring collector reload";
                }
            }
        } else if (event.event == "insert") {
            if (event.type == ElementType::Package) {
                insertPackage(event.id);
            } else {
                insertFile(event.id);
            }
        } else if (event.event == "remove") {
            if (event.type == ElementType::Package) {
                emit removePackage(event.id);
            } else {
                emit removeFile(event.id);
            }
        } else if (event.event == "update") {
            if (event.type == ElementType::Package) {
                PackageData p;
                proxy->getPackageData(p, event.id);
                p.links.clear();
                emit updatePackage(p);
            } else {
                FileData f;
                proxy->getFileData(f, event.id);
                emit updateFile(f);
            }
        }
    }

    std::vector<Pyload::DownloadInfo> downloads;
    proxy->statusDownloads(downloads);

    foreach (Pyload::DownloadInfo info, downloads) {
        emit updateDownloadStatus(info);
    }

    timer.setInterval(settings.value("eventloop/updatetime", QVariant(1000)).toInt());
    timer.start();
}

void EventLoop::updateFiles(int id)
{
    std::map<int16_t, FileID> fileorder;
    proxy->getFileOrder(fileorder, id);

    std::pair<int16_t, FileID> pair;
    foreach (pair, fileorder)
    {
        FileData f;
        proxy->getFileData(f, pair.second);
        emit addFile(f);
    }
}

void EventLoop::reloadQueue()
{
    QMutexLocker locker(&mutex);

    std::vector<PackageInfo> packageinfos;
    proxy->getQueue(packageinfos);

    foreach (PackageInfo p, packageinfos) {
        emit addPackage(p);
        updateFiles(p.pid);
    }
}

void EventLoop::reloadCollector()
{
    QMutexLocker locker(&mutex);

    std::vector<PackageInfo> packageinfos;
    proxy->getCollector(packageinfos);

    foreach (PackageInfo p, packageinfos) {
        emit addPackage(p);
        updateFiles(p.pid);
    }
}

void EventLoop::insertPackage(int id)
{
    PackageData p;
    proxy->getPackageData(p, id);
    p.links.clear();

    emit addPackage(p);
    updateFiles(id);
}

void EventLoop::insertFile(int id)
{
    FileData f;
    proxy->getFileData(f, id);
    emit addFile(f);
}