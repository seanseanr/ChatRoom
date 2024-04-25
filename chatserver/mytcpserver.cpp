#include "mytcpserver.h"

MyTcpServer::MyTcpServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);

    server->listen(QHostAddress::LocalHost, 23);

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void MyTcpServer::newConnection()
{
    QTcpSocket* socket = server->nextPendingConnection();
    QString cred = socket->readAll();
    QString response = isValid(cred) ? "Log in successful!!" : cred + " already exists!!";
    socket->write(response.toUtf8());
    socket->flush();
    socket->waitForBytesWritten();
    if(isValid(cred))
    {
        clientserverMap[cred] = socket;
    }
    else
    {
        socket->close();
    }
}

bool MyTcpServer::isValid(QString &cred)
{
    return (clientserverMap.count(cred) > 0 && (server_name == cred)) ? false : true;
}

void MyTcpServer::freeconnections()
{
    for(auto it = clientserverMap.begin(); it != clientserverMap.end(); it++)
    {
        //it->second.close();
    }
}
