#include "chatserver.h"
#include "mainwindow.h"
#include <QTcpSocket>
#include <QString>
#include <QTimer>
#include <QEventLoop>
#include <QElapsedTimer>
#include <QCoreApplication>

class MainWindow;
extern MainWindow* w;

ChatServer::ChatServer(QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::Any, 5000);
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
                tokdat.append(client->readAll());

                if(tokdat.indexOf(LOGIN_SIGN) == 0)
                {
                    QString username = QString::fromUtf8(tokdat.data() + SIGN_LEN - 1);
                    users[client].first = username;
                    users[client].second = false;
                    tokdat.clear();
                }
                else
                {
                    if(tokdat.indexOf(TS_PIC_SIGN) == 0)
                    {
                        num_s = QString(tokdat.data() + TS_PIC_SIGN_LEN - 1);
                        qDebug()<<"num_s:"<<num_s;
                        expected_bytes = num_s.toInt() + TS_PIC_SIGN_LEN + num_s.length();
                        written_size = tokdat.size();
                        //ba.setRawData(tokdat.data() + TS_PIC_SIGN_LEN + num_s.length(), written_size);
                        for(QTcpSocket *socket: clients)
                        {
                            socket->write(QString(users[client].first + ": ").toUtf8());
                            socket->flush();
                            socket->waitForBytesWritten();
                        }
                        users[client].second = true;
                        break;
                    }
                    else
                    {
                        QString s = users[client].first + ": " + QString::fromUtf8(tokdat.data());
                        w->appendEditorFromclient(s);
                        foreach(QTcpSocket *socket, clients)
                        {
                            QString username_ = users[client].first + ": ";
                            socket->write(s.toUtf8());
                        }
                        tokdat.clear();
                    }
                }
            }while(client->bytesAvailable());
        }
        if(users[client].second)
        {
            while(client->bytesAvailable())
            {
                written_size += client->bytesAvailable();
                qDebug()<<"Bytes written:"<<written_size;
                tokdat.append(client->readAll());

            }
            if(written_size >= expected_bytes)
            {
                users[client].second = false;
                dispatchPic(tokdat);
                //ba.clear();
                tokdat.clear();
            }
        }
}

void ChatServer::dispatchPic(QByteArray ba)
{
    foreach(QTcpSocket *client, clients)
    {
        //tokdat.clear();
        //QString num = QString("%1").arg(ba.size());
        //qDebug()<<"dispatchPic:"<<num;
        //tokdat.append(QString(TS_PIC_SIGN + num + '\0'));
        //tokdat.append(ba);
        client->write(tokdat);
        client->flush();
        client->waitForBytesWritten();
    }
}

void ChatServer::qt_wait_ms(float amount)
{
#if 1
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<amount*1000)
    {
        //QCoreApplication::processEvents();
    }
#else
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(amount*1);
    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start();
    loop.exec();
#endif
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
