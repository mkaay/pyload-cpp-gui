#include "file.h"

File::File() : QObject()
{
    speed = 0;
    eta = 0;
    waituntil = 0;
    progress = 0;
    downloadedSize = 0;
}

void File::parse(Pyload::FileData &data)
{
    QMutexLocker locker(&mutex);
    id = data.fid;
    url = QUrl(QString::fromStdString(data.name));
    name = QString::fromStdString(data.name);
    plugin = QString::fromStdString(data.plugin);
    size = data.size;
    status = static_cast<FileStatus>(data.status);
    error = QString::fromStdString(data.error);
    //progress = data.progress;
    order = data.order;

    emit update(id);
}

int File::getID()
{
    QMutexLocker locker(&mutex);
    return id;
}

void File::setID(int id)
{
    QMutexLocker locker(&mutex);
    this->id = id;
}

QString File::getName()
{
    QMutexLocker locker(&mutex);
    return name;
}

void File::setName(QString name)
{
    QMutexLocker locker(&mutex);
    this->name = name;
}

QUrl File::getUrl()
{
    QMutexLocker locker(&mutex);
    return url;
}

QString File::getPlugin()
{
    QMutexLocker locker(&mutex);
    return plugin;
}

long File::getSize()
{
    QMutexLocker locker(&mutex);
    return size;
}

void File::setSize(long size)
{
    QMutexLocker locker(&mutex);
    this->size = size;
}

long File::getDownloadedSize()
{
    if (getProgress() == 100) {
        return getSize();
    }
    QMutexLocker locker(&mutex);
    return downloadedSize;
}

void File::setDownloadedSize(long size)
{
    QMutexLocker locker(&mutex);
    downloadedSize = size;
}

FileStatus File::getStatus()
{
    QMutexLocker locker(&mutex);
    return status;
}

void File::setStatus(FileStatus status)
{
    QMutexLocker locker(&mutex);
    this->status = status;
}

QString File::getError()
{
    QMutexLocker locker(&mutex);
    return error;
}

short File::getProgress()
{
    QMutexLocker locker(&mutex);
    locker.unlock();
    if (getStatus() == Finished || getStatus() == Skipped) {
        return 100;
    }
    locker.relock();
    return progress;
}

void File::setProgress(short progress)
{
    QMutexLocker locker(&mutex);
    this->progress = progress;
}

int File::getSpeed()
{
    QMutexLocker locker(&mutex);
    if (status != Downloading) {
        return 0;
    }
    return speed;
}

void File::setSpeed(int speed)
{
    QMutexLocker locker(&mutex);
    this->speed = speed;
}

short File::getOrder()
{
    QMutexLocker locker(&mutex);
    return order;
}

void File::incrementOrder()
{
    QMutexLocker locker(&mutex);
    order++;
}

void File::decrementOrder()
{
    QMutexLocker locker(&mutex);
    order--;
}

bool File::operator <(File &other)
{
    return getOrder() < other.getOrder();
}

void File::setPackage(Package *p)
{
    QMutexLocker locker(&mutex);
    package = p;
}

Package* File::getPackage()
{
    QMutexLocker locker(&mutex);
    return package;
}

int File::getETA()
{
    QMutexLocker locker(&mutex);
    if (status != Downloading) {
        return 0;
    }
    return eta;
}

void File::setETA(int eta)
{
    QMutexLocker locker(&mutex);
    this->eta = eta;
}

int File::getWaitUntil()
{
    QMutexLocker locker(&mutex);
    if (status != Waiting) {
        return 0;
    }
    return waituntil;
}

void File::setWaitUntil(int until)
{
    QMutexLocker locker(&mutex);
    waituntil = until;
}
