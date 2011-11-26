#include "package.h"

#include <QDebug>

#include "file.h"

Package::Package()
{

}

void Package::parse(Pyload::PackageData &data)
{
    QMutexLocker locker(&mutex);
    id = data.pid;
    name = QString::fromStdString(data.name);
    folder = QString::fromStdString(data.folder);
    site = QUrl(QString::fromStdString(data.site));
    password = QString::fromStdString(data.password);
    if (data.dest == Pyload::Destination::Queue) { // fixme: switched!
        active = false;
    } else {
        active = true;
    }
    priorty = data.priority;
    order = data.order;
}

void Package::parse(Pyload::PackageInfo &data)
{
    QMutexLocker locker(&mutex);
    id = data.pid;
    name = QString::fromStdString(data.name);
    folder = QString::fromStdString(data.folder);
    site = QUrl(QString::fromStdString(data.site));
    password = QString::fromStdString(data.password);
    if (data.dest == Pyload::Destination::Queue) { // fixme: switched!
        active = false;
    } else {
        active = true;
    }
    priorty = data.priority;
    order = data.order;
}

void Package::addFile(File *f)
{
    QMutexLocker locker(&mutex);
    files.append(f);
}

void Package::clearFiles()
{
    QMutexLocker locker(&mutex);
    files.clear();
}

void Package::insertFile(int position, File *f)
{
    QMutexLocker locker(&mutex);
    files.insert(position, f);
}

void Package::removeFile(File *f)
{
    QMutexLocker locker(&mutex);
    files.removeOne(f);
}

bool Package::removeFile(int id)
{
    QMutexLocker locker(&mutex);
    foreach (File *f, files) {
        if (f->getID() == id) {
            files.removeOne(f);
            return true;
        }
    }
    return false;
}

bool Package::contains(int id)
{
    QMutexLocker locker(&mutex);
    foreach (File *f, files) {
        if (f->getID() == id) {
            return true;
        }
    }
    return false;
}

int Package::indexOf(int id)
{
    QMutexLocker locker(&mutex);
    foreach (File *f, files) {
        if (f->getID() == id) {
            return files.indexOf(f);
        }
    }
    return -1;
}

int Package::getID()
{
    QMutexLocker locker(&mutex);
    return id;
}

void Package::setID(int id)
{
    QMutexLocker locker(&mutex);
    this->id = id;
}

QString Package::getName()
{
    QMutexLocker locker(&mutex);
    return name;
}

QStringList Package::getPlugins()
{
    QMutexLocker locker(&mutex);
    QStringList plugins;
    foreach (File *file, files) {
        if (!plugins.contains(file->getPlugin())) {
            plugins << file->getPlugin();
        }
    }
    return plugins;
}

long Package::getSize()
{
    QMutexLocker locker(&mutex);
    long size = 0;
    foreach (File *file, files) {
        size += file->getSize();
    }
    return size;
}

long Package::getDownloadedSize()
{
    QMutexLocker locker(&mutex);
    long size = 0;
    foreach (File *file, files) {
        size += file->getDownloadedSize();
    }
    return size;
}

int Package::getStatus()
{
    QMutexLocker locker(&mutex);
    FileStatus status = Finished;
    foreach (File *file, files) {
        if (status < file->getStatus()) {
            status = file->getStatus();
        }
    }
    return status;
}

short Package::getProgress()
{
    QMutexLocker locker(&mutex);
    int progress = 0;
    foreach (File *file, files) {
        progress += file->getProgress();
    }
    if (files.size() == 0) {
        return 0;
    }
    return static_cast<short>(progress/files.size());
}

int Package::getSpeed()
{
    QMutexLocker locker(&mutex);
    int speed = 0;
    foreach (File *file, files) {
        speed += file->getSpeed();
    }
    return speed;
}

QString Package::getPassword()
{
    QMutexLocker locker(&mutex);
    return password;
}

bool Package::isActive()
{
    QMutexLocker locker(&mutex);
    return active;
}

short Package::getPriority()
{
    QMutexLocker locker(&mutex);
    return priorty;
}

QUrl Package::getDownloadSource()
{
    QMutexLocker locker(&mutex);
    return site;
}

short Package::getOrder()
{
    QMutexLocker locker(&mutex);
    return order;
}

void Package::incrementOrder()
{
    QMutexLocker locker(&mutex);
    order++;
}

void Package::decrementOrder()
{
    QMutexLocker locker(&mutex);
    order--;
}

void Package::sortFiles()
{
    QMutexLocker locker(&mutex);
    qSort(files);
}

bool Package::operator <(Package &other)
{
    if (isActive() == other.isActive()) {
        return getOrder() < other.getOrder();
    } else if (isActive() && !other.isActive()) {
        return true;
    } else {
        return false;
    }
}

File* Package::getFile(int row)
{
    QMutexLocker locker(&mutex);
    return files.at(row);
}

int Package::fileCount()
{
    QMutexLocker locker(&mutex);
    if (files.isEmpty()) {
        return 0;
    }
    return files.size();
}

int Package::getETA()
{
    if (getSpeed() == 0) {
        return 0;
    }
    if (getSize() - getDownloadedSize() <= 0) {
        return 0;
    }
    return (getSize() - getDownloadedSize()) / getSpeed();
}
