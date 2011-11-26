#ifndef DOWNLOADSMODEL_H
#define DOWNLOADSMODEL_H

#include "../thrift/interface/Pyload.h"

#include <QAbstractItemModel>
#include <QBrush>
#include <QColor>
#include <QSharedPointer>

#include "../package.h"
#include "../file.h"

using namespace Pyload;

struct Wrap {
    bool isPackage;
    union {
        Package* package;
        File* file;
    };
};

class DownloadsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DownloadsModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QString formatSize(long size) const;
    QString formatETA(int eta) const;
    QString formatStatus(FileStatus status) const;

    bool isPackage(const QModelIndex &index) const;
    Package* packageFromIndex(const QModelIndex &index) const;
    File* fileFromIndex(const QModelIndex &index) const;

    Package* getPackage(int row) const;
    int packageCount() const;

public slots:
    void addFile(Pyload::FileData &f);
    void addPackage(Pyload::PackageData &p);
    void addPackage(Pyload::PackageInfo &p);
    void removeFile(int id);
    void removePackage(int id);
    void updatePackage(Pyload::PackageData &p);
    void updateFile(Pyload::FileData &f);
    void updateDownloadStatus(Pyload::DownloadInfo &info);

private:
    static QStringList sections;
    QMap<Package*, Wrap*> *package_wraps;
    QMap<File*, Wrap*> *file_wraps;
    QList<Package*> packages;
    QMutex mutex;
};

#endif // DOWNLOADSMODEL_H
