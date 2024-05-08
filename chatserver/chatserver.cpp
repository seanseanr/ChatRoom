#include "chatserver.h"
#include "mainwindow.h"
#include <QTcpSocket>
#include <QString>

class MainWindow;
extern MainWindow* w;

ChatServer::ChatServer(QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::LocalHost, 5000);
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
    do
    {
        QString line = QString::fromUtf8(client->readLine());

        if(line.contains(LOGIN_SIGN))
        {
            QString username = QString(line.data() + SIGN_LEN - 1);
            users[client] = username;
            w->picserver->add_username(username);
        }
        else
        {
            QString s = users[client] + ": " + line;
            w->appendEditorFromclient(s);
            foreach(QTcpSocket *socket, clients)
            {
                //if(socket != client)
                {
                    QString username_ = users[client] + ": ";
                    socket->write((username_+line).toAscii());
                }
            }
        }
    }while(client->canReadLine());
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
        socket->write(line.toAscii());
    }
}

ChatServer::~ChatServer()
{
    foreach(QTcpSocket *socket, clients)
    {
        disconnect(socket);
    }
}
