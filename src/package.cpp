#include "package.h"

#include <QDebug>

#include "file.h"

Package::Package() : QObject()
{
    size = -1;
    downloadedSize = -1;
    progress = -1;
    speed = -1;
    eta = -1;
    status = -1;
}

Package::~Package() {
    foreach (File *file, files) {
        file->deleteLater();
    }
    files.clear();
    idlookup.clear();
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

void Package::fileUpdated(int id)
{
    size = -1;
    downloadedSize = -1;
    progress = -1;
    speed = -1;
    eta = -1;
    status = -1;
}

void Package::addFile(File *f)
{
    QMutexLocker locker(&mutex);
    files.append(f);
    idlookup.insert(f->getID(), f);
    connect(f, SIGNAL(update(int)), this, SLOT(fileUpdated(int)));
    emit fileUpdated(f->getID());
}

void Package::clearFiles()
{
    QMutexLocker locker(&mutex);
    files.clear();
    idlookup.clear();
}

void Package::insertFile(int position, File *f)
{
    QMutexLocker locker(&mutex);
    files.insert(position, f);
    idlookup.insert(f->getID(), f);
    connect(f, SIGNAL(update(int)), this, SLOT(fileUpdated(int)));
    emit fileUpdated(f->getID());
}

void Package::removeFile(File *f)
{
    QMutexLocker locker(&mutex);
    idlookup.remove(f->getID());
    files.removeOne(f);
}

bool Package::removeFile(int id)
{
    QMutexLocker locker(&mutex);
    if (!idlookup.contains(id)) {
        return false;
    }
    files.removeOne(idlookup.value(id));
    idlookup.remove(id);
    return true;
}

bool Package::contains(int id)
{
    QMutexLocker locker(&mutex);
    return idlookup.contains(id);
}

int Package::indexOf(int id)
{
    QMutexLocker locker(&mutex);
    if (!idlookup.contains(id)) {
        return -1;
    }
    return files.indexOf(idlookup.value(id));
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
    if (size >= 0) {
        return size;
    }
    long s = 0;
    foreach (File *file, files) {
        s += file->getSize();
    }
    size = s;
    return s;
}

long Package::getDownloadedSize()
{
    QMutexLocker locker(&mutex);
    if (downloadedSize >= 0) {
        return downloadedSize;
    }
    long size = 0;
    foreach (File *file, files) {
        size += file->getDownloadedSize();
    }
    downloadedSize = size;
    return size;
}

int Package::getStatus()
{
    QMutexLocker locker(&mutex);
    if (status >= 0) {
        return status;
    }
    int st = Finished;
    foreach (File *file, files) {
        if (st < file->getStatus()) {
            st = file->getStatus();
        }
    }
    status = st;
    return st;
}

short Package::getProgress()
{
    QMutexLocker locker(&mutex);
    if (progress >= 0) {
        return progress;
    }
    int p = 0;
    foreach (File *file, files) {
        p += file->getProgress();
    }
    if (files.size() == 0) {
        return 0;
    }
    progress = static_cast<short>(p/files.size());
    return progress;
}

int Package::getSpeed()
{
    QMutexLocker locker(&mutex);
    if (speed >= 0) {
        return speed;
    }
    int s = 0;
    foreach (File *file, files) {
        s += file->getSpeed();
    }
    speed = s;
    return s;
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
    if (eta >= 0) {
        return eta;
    }
    if (getSpeed() == 0) {
        return 0;
    }
    if (getSize() - getDownloadedSize() <= 0) {
        return 0;
    }
    eta = (getSize() - getDownloadedSize()) / getSpeed();
    return eta;
}
