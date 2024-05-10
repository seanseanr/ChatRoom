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

    users[client].first = "UNKNOWN";
    users[client].second = false;
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void ChatServer::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();

    //QString username = users[client];
        if(users[client].second == false)
        {
            do
            {
                QString line = QString::fromUtf8(client->readLine());

                if(line.contains(LOGIN_SIGN))
                {
                    QString username = QString(line.data() + SIGN_LEN - 1);
                    users[client].first = username;
                    users[client].second = false;
                }
                else
                {
                    if(line.contains(TS_PIC_SIGN))
                    {
                        QString tmp_S =QString(line.data() + TS_PIC_SIGN_LEN - 1).toAscii();
                        expected_bytes = tmp_S.toInt();
                        written_size = 0;
                        users[client].second = true;
                        return;
                    }
                    else
                    {
                        QString s = users[client].first + ": " + line;
                        w->appendEditorFromclient(s);
                        foreach(QTcpSocket *socket, clients)
                        {
                            QString username_ = users[client].first + ": ";
                            socket->write((username_+line).toUtf8());
                        }
                    }
                }
            }while(client->canReadLine());
        }
        else
        {
            while(client->bytesAvailable())
            {
                written_size += client->bytesToWrite();
                ba.append(client->readAll());

            };
            if(written_size >= expected_bytes)
            {
                dispatchPic(ba);
                users[client].second = false;
                ba.clear();
            }
        }
}

void ChatServer::dispatchPic(QByteArray ba)
{
    foreach(QTcpSocket *client, clients)
    {
        client->write(ba);
        client->flush();
        client->waitForBytesWritten();
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
