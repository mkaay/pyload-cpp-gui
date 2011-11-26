#ifndef PACKAGE_H
#define PACKAGE_H

#include <QObject>
#include <QList>
#include <QUrl>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>

#include "thrift/interface/Pyload.h"

class File;

class Package
{
public:
    explicit Package();
    void parse(Pyload::PackageData &data);
    void parse(Pyload::PackageInfo &data);
    void addFile(File *f);
    void insertFile(int position, File *f);
    bool removeFile(int id);
    bool contains(int id);
    int indexOf(int id);
    void removeFile(File *f);
    void clearFiles();
    File* getFile(int row);
    int fileCount();

    int getID();
    void setID(int id);

    QString getName();
    QStringList getPlugins();
    long getSize();
    long getDownloadedSize();
    int getStatus();
    short getProgress();
    int getSpeed();
    QString getPassword();
    bool isActive();
    short getPriority();
    QUrl getDownloadSource();
    short getOrder();
    void incrementOrder();
    void decrementOrder();
    int getETA();

    void sortFiles();
    bool operator <(Package &other);

private:
    QMutex mutex;

    int id;
    QString name;
    QString folder;
    QUrl site;
    QString password;
    bool active;
    short priorty;
    short order;

    QList<File*> files;
};

#endif // PACKAGE_H
