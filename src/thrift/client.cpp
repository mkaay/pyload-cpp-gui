#include "client.h"

#include <QDebug>

ThriftClient::ThriftClient() :
    QObject()
{
    connectionOK = false;
    connect(this, SIGNAL(_doConnect(QString,int,QString,QString)), this, SLOT(_connect(QString,int,QString,QString)));
    connect(this, SIGNAL(disconnect()), this, SLOT(_disconnect()));
}

void ThriftClient::doConnect(QString host, int port, QString user, QString password)
{
    emit _doConnect(host, port, user, password);
}

void ThriftClient::_connect(QString host, int port, QString user, QString password)
{
    socket = new boost::shared_ptr<TSocket>(new TSocket(host.toStdString(), port));
    transport = new boost::shared_ptr<TTransport>(new TBufferedTransport(*socket));
    protocol = new boost::shared_ptr<TProtocol>(new TBinaryProtocol(*transport));

    client = new PyloadClient(*protocol);

    try
    {
        transport->get()->open();
    }
    catch (TTransportException)
    {
        error = ConnectionError;
        return;
    }

    try
    {
        if (client->login(user.toStdString(), password.toStdString()))
        {
            std::string version;
            client->getServerVersion(version);

            if (SERVER_VERSION == version)
            {
                connectionOK = true;
                qDebug() << "login ok";
                error = NoError;
            }
            else
            {
                error = WrongVersion;
            }
        }
        else
        {
            error = BadCredentials;
        }
    }
    catch (TTransportException)
    {
        error = ConnectionError;
    }

    emit connected(connectionOK);
}

void ThriftClient::_disconnect()
{
    transport->get()->close();
    connectionOK = false;
}

PyloadClient* ThriftClient::getProxy()
{
    return client;
}
