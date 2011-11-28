#include "client.h"

#include <QDebug>

ThriftClient::ThriftClient() :
    QObject()
{
    connectionOK = false;
    error = NoError;
    connect(this, SIGNAL(_doConnect(QString,int,QString,QString)), this, SLOT(_connect(QString,int,QString,QString)));
}

void ThriftClient::doConnect(QString host, int port, QString user, QString password)
{
    emit _doConnect(host, port, user, password);
}

void ThriftClient::_connect(QString host, int port, QString user, QString password)
{
    if (host.length() == 0 || port == 0) {
        error = ConnectionError;
    } else {
        try {
            socket = new boost::shared_ptr<TSocket>(new TSocket(host.toStdString(), port));
        } catch (TException) {
            qDebug() << "socket error";
            error = ConnectionError;
        }

        if (error == NoError) {
            transport = new boost::shared_ptr<TTransport>(new TBufferedTransport(*socket));
            protocol = new boost::shared_ptr<TProtocol>(new TBinaryProtocol(*transport));

            client = new PyloadClient(*protocol);

            try {
                transport->get()->open();
            } catch (TTransportException) {
                error = ConnectionError;
            }
        }

        if (error == NoError) {
            try {
                if (client->login(user.toStdString(), password.toStdString())) {
                    std::string version;
                    client->getServerVersion(version);

                    if (SERVER_VERSION == version) {
                        connectionOK = true;
                        qDebug() << "login ok";
                        error = NoError;
                    } else {
                        error = WrongVersion;
                    }
                } else {
                    error = BadCredentials;
                }
            } catch (TTransportException) {
                error = ConnectionError;
            }
        }
    }

    emit connected(connectionOK);
}

void ThriftClient::disconnect()
{
    transport->get()->close();
    connectionOK = false;
}

PyloadClient* ThriftClient::getProxy()
{
    return client;
}
