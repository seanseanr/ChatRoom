#include "pictureserver.h"
#include <QTcpSocket>
#include <QFile>
pictureserver::pictureserver(QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::LocalHost, 5050);
}

void pictureserver::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    users[client] = pendingUserName;
}

void pictureserver::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    do
    {
        written_size += client->bytesToWrite();
        ba.append(client->readAll());

    }while(client->bytesAvailable() > 0);
    if(written_size >= expected_bytes)
    {
        QFile img("D:/server_tmp.png");
        img.open(QFile::Truncate | QFile::WriteOnly);
        img.write(ba);
        ba.clear();
    }
}

void pictureserver::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    clients.remove(client);

    users.remove(client);
}

void pictureserver::dispatchPic(QByteArray &ba)
{
    for(QTcpSocket *client: clients)
    {
        expected_bytes = ba.size();
        written_size = 0;
        client->write(ba);
        client->flush();
        client->waitForBytesWritten();
    }
}

pictureserver::~pictureserver()
{
    foreach(QTcpSocket *socket, clients)
    {
        disconnect(socket);
    }
}
