#include "thriftclient.h"

ThriftClient::ThriftClient()
{
    connected = false;
}

bool ThriftClient::connect(QString host, int port, QString user, QString password)
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
        return false;
    }

    try
    {
        if (client->login(user.toStdString(), password.toStdString()))
        {
            std::string version;
            client->getServerVersion(version);

            if (SERVER_VERSION == version)
            {
                connected = true;
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

    return connected;
}

void ThriftClient::disconnect()
{
    transport->get()->close();
    connected = false;
}

PyloadClient* ThriftClient::getProxy()
{
    return client;
}
