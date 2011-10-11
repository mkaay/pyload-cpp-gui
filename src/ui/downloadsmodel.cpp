#include "downloadsmodel.h"

#include <QDebug>

DownloadsModel::DownloadsModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}

QStringList DownloadsModel::sections = (QStringList() << "Name" << "Status" << "Plugin" << "Priority" << "Size" << "ETA" << "Progress");

QModelIndex DownloadsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < rowCount(parent) && column < columnCount(parent)) {
        if (!parent.isValid()) {
            ItemWrap *wrap = new ItemWrap();
            wrap->type = Package;
            wrap->parentRow = row;
            if (row < queue.size()) {
                wrap->destination = Queue;
                wrap->pointer = &queue[wrap->parentRow];
            } else {
                wrap->destination = Collector;
                wrap->pointer = &collector[wrap->parentRow - queue.size()];
            }

            return createIndex(row, column, wrap);
        } else {
            ItemWrap *parentwrap = (ItemWrap*) parent.internalPointer();
            ItemWrap *wrap = new ItemWrap();
            wrap->type = File;
            wrap->parentRow = parentwrap->parentRow;
            wrap->destination = parentwrap->destination;
            if (wrap->parentRow < queue.size()) {
                wrap->pointer = &queue[wrap->parentRow].links[row];
            } else {
                wrap->pointer = &collector[wrap->parentRow - queue.size()].links[row];
            }

            return createIndex(row, column, wrap);
        }
    }

    return QModelIndex();
}


QModelIndex DownloadsModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    ItemWrap *childwrap = (ItemWrap*) child.internalPointer();
    if (childwrap->type == Package) {
        return QModelIndex();
    } else {
        ItemWrap *wrap = new ItemWrap();
        wrap->type = Package;
        wrap->parentRow = childwrap->parentRow;
        wrap->destination = childwrap->destination;
        if (wrap->destination == Queue) {
            wrap->pointer = &queue[childwrap->parentRow];
        } else {
            wrap->pointer = &collector[childwrap->parentRow - queue.size()];
        }
        return createIndex(childwrap->parentRow, child.column(), wrap);
    }
}


int DownloadsModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return queue.size() + collector.size();
    } else {
        ItemWrap *wrap = (ItemWrap*) parent.internalPointer();
        if (wrap->type == Package) {
            PackageData *package = (PackageData*) wrap->pointer;
            return package->links.size();
        } else {
            return 0;
        }
    }
}


int DownloadsModel::columnCount(const QModelIndex &parent) const
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
            ItemWrap *wrap = (ItemWrap*) index.internalPointer();
            PackageData *package = (PackageData*) wrap->pointer;
            if (index.column() == 0) {
                return QVariant(QString::fromStdString(package->name));
            } else if (index.column() == 1) {
                FileData f;
                QString statusmsg;
                int status = 0;
                foreach (f, package->links) {
                    if (f.status >= status) {
                        status = f.status;
                        statusmsg = QString::fromStdString(f.statusmsg);
                    }
                }
                return QVariant(statusmsg);
            } else if (index.column() == 2) {
                FileData f;
                QStringList plugins;
                foreach (f, package->links) {
                    if (!plugins.contains(QString::fromStdString(f.plugin))) {
                        plugins << QString::fromStdString(f.plugin);
                    }
                }
                return QVariant(plugins.join(", "));
            } else if (index.column() == 3) {
                return QVariant(package->priority);
            } else if (index.column() == 4) {
                long size = 0;
                FileData f;
                foreach (f, package->links) {
                    size += f.size;
                }
                return QVariant(formatSize(size));
            } else if (index.column() == 6) {
                FileData f;
                int progress;
                int count;
                foreach (f, package->links) {
                    if (f.status == 0 || f.status == 4) {
                        progress += 100;
                    } else {
                        progress += f.progress;
                    }
                    count++;
                }
                return QVariant(progress / count);
            }
        } else {
            ItemWrap *wrap = (ItemWrap*) index.internalPointer();
            FileData *file = (FileData*) wrap->pointer;
            if (index.column() == 0) {
                return QVariant(QString::fromStdString(file->name));
            } else if (index.column() == 1) {
                return QVariant(QString::fromStdString(file->statusmsg));
            } else if (index.column() == 2) {
                return QVariant(QString::fromStdString(file->plugin));
            } else if (index.column() == 4) {
                return QVariant(QString::fromStdString(file->format_size));
            } else if (index.column() == 6) {
                if (file->statusmsg == "waiting") {
                    return QVariant(0);
                } else {
                    if (file->status == 0 || file->status == 4) {
                        return QVariant(100);
                    }
                    return QVariant(file->progress);
                }
            }
        }
    } else if (role == Qt::ForegroundRole) {
        ItemWrap *wrap = (ItemWrap*) index.internalPointer();
        if (wrap->destination == Collector) {
            return QVariant(QBrush(QColor("grey")));
        } else {
            return QVariant(QBrush(QColor("black")));
        }
    } else if (role == Qt::ToolTipRole || role == Qt::StatusTipRole) {
        if (!parent(index).isValid()) {
            ItemWrap *wrap = (ItemWrap*) index.internalPointer();
            PackageData *package = (PackageData*) wrap->pointer;
            if (index.column() == 0) {
                return QVariant(QString::fromStdString(package->name));
            }
        } else {
            ItemWrap *wrap = (ItemWrap*) index.internalPointer();
            FileData *file = (FileData*) wrap->pointer;
            if (index.column() == 0) {
                return QVariant(QString::fromStdString(file->name));
            } else if (index.column() == 1) {
                return QVariant(QString::fromStdString(file->error));
            }
        }
    } else if (role == Qt::UserRole) {
        if (parent(index).isValid()) {
            ItemWrap *wrap = (ItemWrap*) index.internalPointer();
            FileData *file = (FileData*) wrap->pointer;
            if (index.column() == 6) {
                if (file->statusmsg == "waiting") {
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


void DownloadsModel::initQueue(std::vector<PackageData> queue)
{
    if (this->queue.size() > 0) {
        return;

        //beginRemoveRows(QModelIndex(), 0, this->queue.size());
        //this->queue.clear();
        //endRemoveRows();
    }
    beginInsertRows(QModelIndex(), 0, queue.size());
    this->queue = queue;
    endInsertRows();
}


void DownloadsModel::initCollector(std::vector<PackageData> collector)
{
    if (this->collector.size() > 0) {
        return;

        //beginRemoveRows(QModelIndex(), queue.size(), queue.size() + this->collector.size());
        //this->collector.clear();
        //endRemoveRows();
    }
    beginInsertRows(QModelIndex(), queue.size(), queue.size() + collector.size());
    this->collector = collector;
    endInsertRows();
}

void DownloadsModel::insertPackage(ItemDestination destination, PackageData package)
{
    std::vector<PackageData> *dest;
    int16_t offset = 0;

    if (destination == Queue) {
        dest = &queue;
    } else {
        dest = &collector;
        offset = queue.size();
    }

    PackageData p;
    int16_t position = 0;
    foreach (p, *dest) {
        if (p.order < package.order) {
            position++;
        } else {
            p.order++;
        }
    }

    beginInsertRows(QModelIndex(), offset + position, offset + position + 1);
    dest->insert(dest->begin() + position, package);
    endInsertRows();
}

void DownloadsModel::insertFile(ItemDestination destination, FileData file)
{
    std::vector<PackageData> *dest;

    if (destination == Queue) {
        dest = &queue;
    } else {
        dest = &collector;
    }

    PackageData package;
    int i = 0;
    foreach (package, *dest) {
        if (package.pid == file.packageID) {
            FileData f;
            int16_t position = 0;
            foreach (f, package.links) {
                if (f.order < file.order) {
                    position++;
                } else {
                    f.order++;
                }
            }

            beginInsertRows(index(i, 0, QModelIndex()), position, position + 1);
            dest->at(i).links.insert(dest->at(i).links.begin() + position, file);
            endInsertRows();

            break;
        }
        i++;
    }
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
