#include "pictureserver.h"
#include <QTcpSocket>

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
        written_size += client->bytesAvailable();
        ba.append(client->readAll());

    }while(client->bytesAvailable() > 0);
    if(written_size >= expected_bytes)
    {
        QImage img("D:/server_tmp.png");
        img.fromData(ba,"PNG");
        img.save("D:/server_tmp.png");
        ba.clear();
    }
}

void pictureserver::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    clients.remove(client);

    users.remove(client);
}

pictureserver::~pictureserver()
{
    foreach(QTcpSocket *socket, clients)
    {
        disconnect(socket);
    }
}
