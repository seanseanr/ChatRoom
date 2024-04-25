#include "chatserver.h"
#include <QTcpSocket>
#include <QString>
ChatServer::ChatServer(QObject *parent) :
    QTcpServer(parent)
{
}

void ChatServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void ChatServer::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    //QString username = users[client];
    while(client->canReadLine())
    {
        QString line = QString::fromUtf8(client->readLine()).trimmed();

        if(line.contains(LOGIN_SIGN))
        {
            QString username = QString(line.data() + SIGN_LEN);
            users[client] = username;
        }
        else
        {
            QString username_ = users[client] + ": ";
            foreach(QTcpSocket *socket, clients)
            {
                socket->write(QString(username_ + line).toUtf8());
            }
        }
    }
}

void ChatServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    clients.remove(client);

    users.remove(client);
}

void ChatServer::dispatchLine(QString line)
{
    foreach(QTcpSocket *socket, clients)
    {
        socket->write(line.toUtf8());
    }
}

ChatServer::~ChatServer()
{
    foreach(QTcpSocket *socket, clients)
    {
        disconnect(socket);
    }
}
