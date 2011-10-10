#include "downloadsmodel.h"

#include <QDebug>

DownloadsModel::DownloadsModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}


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
            qDebug() << package->links.size();
            return package->links.size();
        } else {
            return 0;
        }
    }
}


int DownloadsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
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
            }
        } else {
            ItemWrap *wrap = (ItemWrap*) index.internalPointer();
            FileData *file = (FileData*) wrap->pointer;
            if (index.column() == 0) {
                return QVariant(QString::fromStdString(file->name));
            }
        }
    }

    return QVariant();
}


Qt::ItemFlags DownloadsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


QVariant DownloadsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            QVariant("Name");
        }
    }

    return QVariant();
}


void DownloadsModel::initQueue(std::vector<PackageData> queue)
{
    beginInsertRows(QModelIndex(), 0, queue.size());
    this->queue = queue;
    endInsertRows();
}


void DownloadsModel::initCollector(std::vector<PackageData> collector)
{
    beginInsertRows(QModelIndex(), queue.size(), queue.size() + collector.size());
    this->collector = collector;
    endInsertRows();
}
