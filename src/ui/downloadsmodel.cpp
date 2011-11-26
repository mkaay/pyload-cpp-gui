#include "downloadsmodel.h"

#include <QDebug>

DownloadsModel::DownloadsModel(QObject *parent) : QAbstractItemModel(parent)
{
    package_wraps = new QMap<Package*, Wrap*>();
    file_wraps = new QMap<File*, Wrap*>();
    //packages = new QList<Package*>();
}

QStringList DownloadsModel::sections = (QStringList() << "Name" << "Status" << "Plugin" << "Priority" << "Size" << "ETA" << "Progress");

QModelIndex DownloadsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < rowCount(parent) && column < columnCount(parent)) {
        if (!parent.isValid()) {
            if (package_wraps->contains(getPackage(row))) {
                return createIndex(row, column, package_wraps->value(getPackage(row)));
            }
            Wrap *wrap = new Wrap();
            wrap->isPackage = true;
            wrap->package = getPackage(row);
            package_wraps->insert(getPackage(row), wrap);
            return createIndex(row, column, wrap);
        } else {
            if (file_wraps->contains(packageFromIndex(parent)->getFile(row))) {
                return createIndex(row, column, file_wraps->value(packageFromIndex(parent)->getFile(row)));
            }
            Wrap *wrap = new Wrap();
            wrap->isPackage = false;
            wrap->file = packageFromIndex(parent)->getFile(row);
            file_wraps->insert(packageFromIndex(parent)->getFile(row), wrap);
            return createIndex(row, column, wrap);
        }
    }

    return QModelIndex();
}


QModelIndex DownloadsModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || isPackage(child)) {
        return QModelIndex();
    }
    return index(packages.indexOf(fileFromIndex(child)->getPackage()), 0);
}


int DownloadsModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return packageCount();
    } else if (isPackage(parent)) {
        return packageFromIndex(parent)->fileCount();
    } else {
        return 0;
    }
}


int DownloadsModel::columnCount(const QModelIndex &) const
{
    return sections.length();
}


QVariant DownloadsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        if (!parent(index).isValid()) {
            Package *package = packageFromIndex(index);
            if (index.column() == 0) {
                return package->getName();
            } else if (index.column() == 1) {
                return formatStatus(static_cast<FileStatus>(package->getStatus()));
            } else if (index.column() == 2) {
                return package->getPlugins().join(", ");
            } else if (index.column() == 3) {
                return package->getPriority();
            } else if (index.column() == 4) {
                if (package->getStatus() == Downloading) {
                    return QString("%1/%2").arg(formatSize(package->getDownloadedSize()), formatSize(package->getSize()));
                }
                return formatSize(package->getSize());
            } else if (index.column() == 5) {
                return formatETA(package->getETA());
            } else if (index.column() == 6) {
                return package->getProgress();
            }
        } else {
            File *file = fileFromIndex(index);
            if (index.column() == 0) {
                return file->getName();
            } else if (index.column() == 1) {
                return formatStatus(static_cast<FileStatus>(file->getStatus()));
            } else if (index.column() == 2) {
                return file->getPlugin();
            } else if (index.column() == 4) {
                if (file->getStatus() == Downloading) {
                    return QString("%1/%2").arg(formatSize(file->getDownloadedSize()), formatSize(file->getSize()));
                }
                return formatSize(file->getSize());
            } else if (index.column() == 5) {
                if (file->getStatus() == Waiting) {
                    return formatETA(file->getWaitUntil());
                }
                return formatETA(file->getETA());
            } else if (index.column() == 6) {
                if (file->getStatus() == Waiting) {
                    return 0;
                } else {
                    return file->getProgress();
                }
            }
        }
    } else if (role == Qt::ForegroundRole) {
        Package *package;
        if (!parent(index).isValid()) {
            package = packageFromIndex(index);
        } else {
            package = fileFromIndex(index)->getPackage();
        }

        if (package->isActive()) {
            return QVariant(QBrush(QColor("black")));
        } else {
            return QVariant(QBrush(QColor("grey")));
        }
    } else if (role == Qt::ToolTipRole || role == Qt::StatusTipRole) {
        if (!parent(index).isValid()) {
            if (index.column() == 0) {
                return packageFromIndex(index)->getName();
            }
        } else {
            File *file = fileFromIndex(index);
            if (index.column() == 0) {
                return file->getName();
            } else if (index.column() == 1) {
                return file->getError();
            }
        }
    } else if (role == Qt::UserRole) {
        if (parent(index).isValid()) {
            if (index.column() == 6) {
                if (fileFromIndex(index)->getStatus() == Waiting) {
                    return QVariant(1);
                } else {
                    return QVariant(0);
                }
            }
        } else {
            if (index.column() == 6) {
                return QVariant(0);
            }
        }
    }

    return QVariant();
}


Qt::ItemFlags DownloadsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


QVariant DownloadsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return QVariant(sections[section]);
    }

    return QVariant();
}


QString DownloadsModel::formatSize(long size) const
{
    if (size < 1024) {
        return QString("%1 Byte").arg(QString::number(size));
    } else if (size/1024 < 1024) {
        return QString("%1 KiB").arg(QString::number(size/1024.0, 'f', 2));
    } else if (size/1024/1024 < 1024) {
        return QString("%1 MiB").arg(QString::number(size/1024.0/1024.0, 'f', 2));
    } else {
        return QString("%1 GiB").arg(QString::number(size/1024.0/1024.0/1024.0, 'f', 2));
    }
}

bool DownloadsModel::isPackage(const QModelIndex &index) const
{
    //QMutexLocker locker(&mutex);
    return static_cast<Wrap*>(index.internalPointer())->isPackage;
}

Package* DownloadsModel::packageFromIndex(const QModelIndex &index) const
{
    //QMutexLocker locker(&mutex);
    if (!isPackage(index)) {
        qDebug() << "not a package!";
    }
    return static_cast<Wrap*>(index.internalPointer())->package;
}

File* DownloadsModel::fileFromIndex(const QModelIndex &index) const
{
    if (isPackage(index)) {
        qDebug() << "not a file!";
    }
    //QMutexLocker locker(&mutex);
    return static_cast<Wrap*>(index.internalPointer())->file;
}

Package* DownloadsModel::getPackage(int row) const
{
    //QMutexLocker locker(&mutex);
    return packages.at(row);
}

int DownloadsModel::packageCount() const
{
    //QMutexLocker locker(&mutex);
    return packages.size();
}

void DownloadsModel::addPackage(Pyload::PackageData &p)
{
    //QMutexLocker locker(&mutex);
    Package *package = new Package();
    package->parse(p);
    if (!package->isActive()) {
        beginInsertRows(QModelIndex(), packages.size()-1, packages.size());
        packages.append(package);
    } else {
        foreach (Package *pack, packages) {
            if (package->isActive() && !pack->isActive()) {
                beginInsertRows(QModelIndex(), packages.indexOf(pack), packages.indexOf(pack)+1);
                packages.insert(packages.indexOf(pack), package);
                break;
            }
        }
    }

    endInsertRows();

    beginInsertRows(index(packages.indexOf(package), 0), 0, p.links.size()-1);
    foreach (FileData f, p.links) {
        File *file = new File();
        file->parse(f);
        file->setPackage(package);
        package->addFile(file);
    }
    endInsertRows();
}

void DownloadsModel::addPackage(Pyload::PackageInfo &p)
{
    //QMutexLocker locker(&mutex);
    Package *package = new Package();
    package->parse(p);
    if (!package->isActive()) {
        beginInsertRows(QModelIndex(), packages.size()-1, packages.size());
        packages.append(package);
    } else {
        foreach (Package *pack, packages) {
            if (package->isActive() && !pack->isActive()) {
                beginInsertRows(QModelIndex(), packages.indexOf(pack), packages.indexOf(pack)+1);
                packages.insert(packages.indexOf(pack), package);
                break;
            }
        }
    }
    endInsertRows();
}

void DownloadsModel::addFile(Pyload::FileData &f)
{
    //QMutexLocker locker(&mutex);
    foreach (Package *package, packages) {
        if (package->getID() == f.packageID) {
            beginInsertRows(index(packages.indexOf(package), 0), package->fileCount()-1, package->fileCount());
            File *file = new File();
            file->parse(f);
            file->setPackage(package);
            package->addFile(file);
            endInsertRows();
            break;
        }
    }
}

void DownloadsModel::removePackage(int id)
{
    foreach (Package *package, packages) {
        if (package->getID() == id) {
            beginRemoveRows(QModelIndex(), packages.indexOf(package), packages.indexOf(package)+1);
            packages.removeOne(package);
            endRemoveRows();
            break;
        }
    }
}

void DownloadsModel::removeFile(int id)
{
    foreach (Package *package, packages) {
        if (package->contains(id)) {
            int i = package->indexOf(id);
            beginRemoveRows(index(packages.indexOf(package), 0), i, i+1);
            package->removeFile(i);
            endRemoveRows();
        }
    }
}

void DownloadsModel::updateFile(Pyload::FileData &f)
{
    foreach (Package *package, packages) {
        if (package->contains(f.fid)) {
            int i = package->indexOf(f.fid);
            package->getFile(i)->parse(f);
            dataChanged(index(i, 0), index(i, columnCount()-1));
        }
    }
}

void DownloadsModel::updatePackage(Pyload::PackageData &p)
{
    foreach (Package *package, packages) {
        if (package->getID() == p.pid) {
            package->parse(p);
            dataChanged(index(packages.indexOf(package), 0), index(packages.indexOf(package), columnCount()-1));
            break;
        }
    }
}

void DownloadsModel::updateDownloadStatus(Pyload::DownloadInfo &info)
{
    foreach (Package *package, packages) {
        if (package->contains(info.fid)) {
            int i = package->indexOf(info.fid);
            File *file = package->getFile(i);
            file->setStatus(static_cast<FileStatus>(info.status));
            file->setETA(info.eta);
            file->setName(QString::fromStdString(info.name));
            file->setProgress(info.percent);
            file->setDownloadedSize(file->getSize()-info.bleft);
            file->setSpeed(info.speed);
            file->setWaitUntil(info.wait_until);
            dataChanged(index(i, 0), index(i, columnCount()-1));
        }
    }
}

QString DownloadsModel::formatETA(int seconds) const
{
    QString fmt("");

    short hours = seconds / 3600;
    seconds = seconds % 3600;
    short minutes = seconds / 60;
    seconds = seconds % 60;

    if (minutes == 0 && hours == 0) {
        if (seconds == 0) {
            return fmt;
        }
        fmt.prepend(QString("%1s").arg(QString::number(seconds)));
    } else {
        fmt.prepend(QString("%1").arg(QString::number(seconds), 2, QChar('0')));
    }
    if (minutes > 0) {
        fmt.prepend(":");
        fmt.prepend(QString("%1").arg(QString::number(minutes), 2, QChar('0')));
    }
    if (hours > 0) {
        fmt.prepend(":");
        fmt.prepend(QString("%1").arg(QString::number(hours), 2, QChar('0')));
    }

    return fmt;
}

QString DownloadsModel::formatStatus(FileStatus status) const
{
    if (status == Finished)     return QString("Finished");
    if (status == Offline)      return QString("Offline");
    if (status == Online)       return QString("Online");
    if (status == Queued)       return QString("Queued");
    if (status == Skipped)      return QString("Skipped");
    if (status == Waiting)      return QString("Waiting");
    if (status == TempOffline)  return QString("TempOffline");
    if (status == Starting)     return QString("Starting");
    if (status == Failed)       return QString("Failed");
    if (status == Aborted)      return QString("Aborted");
    if (status == Decrypting)   return QString("Decrypting");
    if (status == Custom)       return QString("Custom");
    if (status == Downloading)  return QString("Downloading");
    if (status == Processing)   return QString("Processing");
    if (status == Unknown)      return QString("Unknown");
}
