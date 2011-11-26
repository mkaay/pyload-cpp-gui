#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QUrl>
#include <QMutex>
#include <QMutexLocker>

#include "thrift/interface/Pyload.h"

#include "package.h"

enum FileStatus {
  Finished = 0,
  Offline = 1,
  Online = 2,
  Queued = 3,
  Skipped = 4,
  Waiting = 5,
  TempOffline = 6,
  Starting = 7,
  Failed = 8,
  Aborted = 9,
  Decrypting = 10,
  Custom = 11,
  Downloading = 12,
  Processing = 13,
  Unknown = 14
};

class File
{
public:
    explicit File();

    void parse(Pyload::FileData &data);

    int getID();
    void setID(int id);

    QUrl getUrl();
    QString getName();
    void setName(QString name);
    QString getPlugin();
    long getSize();
    void setSize(long size);
    long getDownloadedSize();
    void setDownloadedSize(long size);
    FileStatus getStatus();
    void setStatus(FileStatus status);
    QString getError();
    short getProgress();
    void setProgress(short progress);
    int getSpeed();
    void setSpeed(int speed);
    short getOrder();
    void incrementOrder();
    void decrementOrder();
    int getETA();
    void setETA(int eta);
    int getWaitUntil();
    void setWaitUntil(int until);

    void setPackage(Package *p);
    Package* getPackage();

    bool operator <(File &other);

private:
    QMutex mutex;

    int id;
    QUrl url;
    QString name;
    QString plugin;
    long size;
    long downloadedSize;
    FileStatus status;
    QString error;
    short progress;
    int speed;
    short order;
    int eta;
    int waituntil;

    Package *package;
};

#endif // FILE_H
