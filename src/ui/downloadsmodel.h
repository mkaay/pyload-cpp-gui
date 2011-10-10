#ifndef DOWNLOADSMODEL_H
#define DOWNLOADSMODEL_H

#include "../thrift/interface/Pyload.h"

#include <QAbstractItemModel>

using namespace Pyload;

enum ItemType {Package, File};
enum ItemDestination {Queue, Collector};

struct ItemWrap {
    ItemType type;
    ItemDestination destination;
    int parentRow;
    const void *pointer;
};

class DownloadsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DownloadsModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void initQueue(std::vector<PackageData> queue);
    void initCollector(std::vector<PackageData> collector);

signals:

public slots:

private:
    std::vector<PackageData> queue;
    std::vector<PackageData> collector;
};

#endif // DOWNLOADSMODEL_H
