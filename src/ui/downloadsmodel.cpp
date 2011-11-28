#include "downloadsmodel.h"

#include <QDebug>

DownloadsModel::DownloadsModel(QObject *parent) : QAbstractItemModel(parent)
{
    package_wraps = new QMap<Package*, Wrap*>();
    file_wraps = new QMap<File*, Wrap*>();
}

QStringList DownloadsModel::sectionnames = (QStringList() << "Name" << "Status" << "Plugin" << "Size" << "ETA" << "Progress");

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
    return sectionnames.length();
}


QVariant DownloadsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        if (!parent(index).isValid()) {
            Package *package = packageFromIndex(index);
            if (index.column() == Name) {
                return package->getName();
            } else if (index.column() == Status) {
                FileStatus status = static_cast<FileStatus>(package->getStatus());
                if (status == Downloading) {
                    return QString("%1 (%2/s)").arg(formatStatus(status), formatSize(package->getSpeed()));
                }
                return formatStatus(status);
            } else if (index.column() == Size) {
                if (package->getStatus() == Downloading) {
                    return QString("%1/%2").arg(formatSize(package->getDownloadedSize()), formatSize(package->getSize()));
                }
                return formatSize(package->getSize());
            } else if (index.column() == ETA) {
                return formatETA(package->getETA());
            } else if (index.column() == Progress) {
                return package->getProgress();
            }
        } else {
            File *file = fileFromIndex(index);
            if (index.column() == Name) {
                return file->getName();
            } else if (index.column() == Status) {
                FileStatus status = static_cast<FileStatus>(file->getStatus());
                if (status == Downloading) {
                    return QString("%1 (%2/s)").arg(formatStatus(status), formatSize(file->getSpeed()));
                }
                return formatStatus(status);
            } else if (index.column() == Size) {
                if (file->getStatus() == Downloading) {
                    return QString("%1/%2").arg(formatSize(file->getDownloadedSize()), formatSize(file->getSize()));
                }
                return formatSize(file->getSize());
            } else if (index.column() == ETA) {
                return formatETA(file->getETA());
            } else if (index.column() == Progress) {
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
            if (index.column() == Name) {
                return packageFromIndex(index)->getName();
            } else if (index.column() == Plugin) {
                return packageFromIndex(index)->getPlugins().join(", ");
            }
        } else {
            File *file = fileFromIndex(index);
            if (index.column() == Name) {
                return file->getName();
            } else if (index.column() == Status) {
                return file->getError();
            } else if (index.column() == Plugin) {
                return file->getPlugin();
            }
        }
    } else if (role == Qt::UserRole) {
        if (parent(index).isValid()) {
            if (index.column() == Progress) {
                if (fileFromIndex(index)->getStatus() == Waiting) {
                    return QVariant(1);
                } else {
                    return QVariant(0);
                }
            }
        } else {
            if (index.column() == Progress) {
                return QVariant(0);
            }
        }
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 5) {
            return Qt::AlignRight;
        }
    } else if (role == Qt::FontRole) {
        if (index.column() == Name && isPackage(index)) {
            QFont f = QApplication::font();
            f.setPixelSize(14);
            return f;
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == Plugin) {
            if (isPackage(index)) {
                Package *package = packageFromIndex(index);
                QStringList plugins = package->getPlugins();
                if (plugins.size() > 0) {
                    return QIcon(QString(":/hoster/%1.png").arg(plugins.first()));
                }
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
        return QVariant(sectionnames[section]);
    }

    return QVariant();
}


QString DownloadsModel::formatSize(long size, short prec)
{
    if (size < 1024) {
        return QString("%1 Byte").arg(QString::number(size));
    } else if (size/1024 < 1024) {
        return QString("%1 KiB").arg(QString::number(size/1024.0, 'f', prec));
    } else if (size/1024/1024 < 1024) {
        return QString("%1 MiB").arg(QString::number(size/1024.0/1024.0, 'f', prec));
    } else {
        return QString("%1 GiB").arg(QString::number(size/1024.0/1024.0/1024.0, 'f', prec));
    }
}

bool DownloadsModel::isPackage(const QModelIndex &index)
{
    //QMutexLocker locker(&mutex);
    return static_cast<Wrap*>(index.internalPointer())->isPackage;
}

Package* DownloadsModel::packageFromIndex(const QModelIndex &index)
{
    //QMutexLocker locker(&mutex);
    if (!isPackage(index)) {
        qDebug() << "not a package!";
    }
    return static_cast<Wrap*>(index.internalPointer())->package;
}

File* DownloadsModel::fileFromIndex(const QModelIndex &index)
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
    QMutexLocker locker(&mutex);
    Package *package = new Package();
    package->parse(p);
    qDebug() << package->isActive() << package->getOrder();
    if (package->isActive()) {
        beginInsertRows(QModelIndex(), package->getOrder(), package->getOrder()+1);
        packages.insert(package->getOrder(), package);
        idlookup.insert(package->getID(), package);
        endInsertRows();
    } else {
        int off = 0;
        foreach (Package *pack, packages) {
            if (pack->isActive()) {
                off++;
            } else {
                break;
            }
        }

        beginInsertRows(QModelIndex(), off+package->getOrder(), off+package->getOrder()+1);
        packages.insert(off+package->getOrder(), package);
        idlookup.insert(package->getID(), package);
        endInsertRows();
    }

    if (p.links.size() > 0) {
        qDebug() << "files attached";
        if (package->fileCount() > 0) {
            qDebug() << "clearing old files";
            beginRemoveRows(index(packages.indexOf(package), 0), 0, package->fileCount()-1);
            package->clearFiles();
            endRemoveRows();
        }

        beginInsertRows(index(packages.indexOf(package), 0), 0, p.links.size()-1);
        foreach (FileData f, p.links) {
            qDebug() << "adding" << f.fid;
            File *file = new File();
            file->parse(f);
            file->setPackage(package);
            package->addFile(file);
            filelookup.insert(file->getID(), package->getID());
        }
        endInsertRows();
    }
}

void DownloadsModel::addPackage(Pyload::PackageInfo &p)
{
    QMutexLocker locker(&mutex);
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
    idlookup.insert(package->getID(), package);

    endInsertRows();
}

void DownloadsModel::addFile(Pyload::FileData &f)
{
    QMutexLocker locker(&mutex);
    Package *package = idlookup.value(f.packageID);
    beginInsertRows(index(packages.indexOf(package), 0), package->fileCount()-1, package->fileCount());
    File *file = new File();
    file->parse(f);
    file->setPackage(package);
    package->addFile(file);
    filelookup.insert(file->getID(), package->getID());
    endInsertRows();
}

void DownloadsModel::removePackage(int id)
{
    beginRemoveRows(QModelIndex(), packages.indexOf(idlookup.value(id)), packages.indexOf(idlookup.value(id))+1);
    package_wraps->remove(idlookup.value(id));
    packages.removeOne(idlookup.value(id));
    idlookup.remove(id);
    QHashIterator<int, int> i(filelookup);
    while (i.hasNext()) {
        i.next();
        if (i.value() == id) {
            filelookup.remove(i.key());
        }
    }
    endRemoveRows();
}

void DownloadsModel::removeFile(int id)
{
    Package *package = idlookup.value(filelookup.value(id));
    int i = package->indexOf(id);
    beginRemoveRows(index(packages.indexOf(package), 0), i, i+1);
    file_wraps->remove(package->getFile(i));
    package->removeFile(i);
    filelookup.remove(id);
    endRemoveRows();
}

void DownloadsModel::updateFile(Pyload::FileData &f)
{
    if (!filelookup.contains(f.fid)) {
        return;
    }
    Package *package = idlookup.value(filelookup.value(f.fid));
    int i = package->indexOf(f.fid);
    package->getFile(i)->parse(f);
    dataChanged(index(i, 0), index(i, columnCount()-1));
}

void DownloadsModel::updatePackage(Pyload::PackageData &p)
{
    idlookup.value(p.pid)->parse(p);
    dataChanged(index(packages.indexOf(idlookup.value(p.pid)), 0), index(packages.indexOf(idlookup.value(p.pid)), columnCount()-1));
}

void DownloadsModel::updateDownloadStatus(Pyload::DownloadInfo &info)
{
    if (!filelookup.contains(info.fid)) {
        return;
    }
    Package *package = idlookup.value(filelookup.value(info.fid));
    int i = package->indexOf(info.fid);
    File *file = package->getFile(i);
    file->setStatus(static_cast<FileStatus>(info.status));
    file->setETA(info.eta);
    file->setName(QString::fromStdString(info.name));
    file->setProgress(info.percent);
    file->setDownloadedSize(file->getSize()-info.bleft);
    file->setSpeed(info.speed);
    file->setWaitUntil(info.wait_until);
    dataChanged(index(i, 0, index(packages.indexOf(package), 0)), index(i, columnCount()-1, index(packages.indexOf(package), columnCount()-1)));
    package->fileUpdated(info.fid);
    dataChanged(index(packages.indexOf(package), 0), index(packages.indexOf(package), columnCount()-1));
}

QString DownloadsModel::formatETA(int seconds)
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
        fmt.prepend(QString("%1s").arg(QString::number(seconds), 2, QChar('0')));
    }
    if (minutes > 0) {
        fmt.prepend(" ");
        fmt.prepend(QString("%1m").arg(QString::number(minutes), 2, QChar('0')));
    }
    if (hours > 0) {
        fmt.prepend(" ");
        fmt.prepend(QString("%1h").arg(QString::number(hours), 2, QChar('0')));
    }

    return fmt;
}

QString DownloadsModel::formatStatus(FileStatus status)
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
    return QString();
}

void DownloadsModel::disconnected()
{
    foreach (Package *package, packages) {
        package->deleteLater();
    }
    packages.clear();
    idlookup.clear();
    filelookup.clear();
    foreach (Wrap *wrap, package_wraps->values()) {
        delete wrap;
    }
    package_wraps->clear();
    foreach (Wrap *wrap, file_wraps->values()) {
        delete wrap;
    }
    file_wraps->clear();
}
