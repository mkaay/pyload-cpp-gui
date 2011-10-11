#ifndef DOWNLOADSMODEL_H
#define DOWNLOADSMODEL_H

#include "../thrift/interface/Pyload.h"

#include <QAbstractItemModel>
#include <QBrush>
#include <QColor>

using namespace Pyload;

enum ItemType {Package, File};
enum ItemDestination {Queue, Collector};

/*
 * A wrapper is needed, because we need to distinguish between FileData and PackageData structs.
 * The internalPointer of the QModelIndex points to this structure, so we can either cast to FileData or PackageData.
 * Additionally parentRow and destination are helpers to quickly find a parent of a QModelIndex, instead of looping through the queue and collector vectors.
 */
struct ItemWrap {
    ItemType type;
    ItemDestination destination;
    uint16_t parentRow;
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
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void initQueue(std::vector<PackageData> queue);
    void initCollector(std::vector<PackageData> collector);

    void insertPackage(ItemDestination destination, PackageData package);
    void insertFile(ItemDestination destination, FileData file);

    QString formatSize(long size) const;

signals:

public slots:

private:
    static QStringList sections;
    std::vector<PackageData> queue;
    std::vector<PackageData> collector;
};

#endif // DOWNLOADSMODEL_H
