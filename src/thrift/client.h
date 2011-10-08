#ifndef THRIFTCLIENT_H
#define THRIFTCLIENT_H

#include "interface/Pyload.h"

#include "transport/TSocket.h"
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <QString>
#include <QObject>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace Pyload;

#define SERVER_VERSION "0.4.8"

enum FailReason {NoError, ConnectionError, WrongVersion, BadCredentials};

class ThriftClient : public QObject
{
    Q_OBJECT
public:
    explicit ThriftClient();
    FailReason getError();
    PyloadClient* getProxy();

    void doConnect(QString host, int port, QString user, QString password);

signals:
    void _doConnect(QString host, int port, QString user, QString password);
    void disconnect();
    void connected(bool ok);

private slots:
    void _connect(QString host, int port, QString user, QString password);
    void _disconnect();

private:
    bool connectionOK;
    FailReason error;

    boost::shared_ptr<TSocket> *socket;
    boost::shared_ptr<TTransport> *transport;
    boost::shared_ptr<TProtocol> *protocol;

    PyloadClient *client;
};

#endif // THRIFTCLIENT_H
