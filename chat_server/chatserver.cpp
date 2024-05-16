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
                        num_s =QString(line.data() + TS_PIC_SIGN_LEN - 1);
                        //qDebug()<<"tmp_S:"<<tmp_S;
                        expected_bytes = num_s.toInt();
                        written_size = 0;
                        for(QTcpSocket *socket: clients)
                        {
                            socket->write(QString(users[client].first + ": ").toUtf8());
                            socket->flush();
                            socket->waitForBytesWritten();
                        }
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
        if(users[client].second)
        {
            while(client->bytesAvailable())
            {
                written_size += client->bytesAvailable();
                qDebug()<<"Bytes written:"<<written_size;
                ba.append(client->readAll());

            }
            if(written_size >= expected_bytes)
            {
                users[client].second = false;
                dispatchPic(ba);
                ba.clear();
            }
        }
}

void ChatServer::dispatchPic(QByteArray ba)
{
    foreach(QTcpSocket *client, clients)
    {
        QString num = QString("%1").arg(ba.size());
        qDebug()<<"dispatchPic:"<<num;
        client->write(QString(TS_PIC_SIGN + num + '\0').toAscii());
        client->flush();
        client->waitForBytesWritten();
        qt_wait_ms(0.01);
        client->write(ba);
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
