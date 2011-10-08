#ifndef THRIFTCLIENT_H
#define THRIFTCLIENT_H

#include "thriftinterface/Pyload.h"

#include "thrift/transport/TSocket.h"
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <QtCore/QString>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace Pyload;

#define SERVER_VERSION "0.4.8"

enum FailReason {NoError, ConnectionError, WrongVersion, BadCredentials};

class ThriftClient
{
public:
    ThriftClient();
    bool connect(QString host, int port, QString user, QString password);
    void disconnect();
    FailReason getError();
    PyloadClient* getProxy();

private:
    bool connected;
    FailReason error;

    boost::shared_ptr<TSocket> *socket;
    boost::shared_ptr<TTransport> *transport;
    boost::shared_ptr<TProtocol> *protocol;

    PyloadClient *client;
};

#endif // THRIFTCLIENT_H
