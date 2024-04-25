#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QMap>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = 0);
    QMap<QString&, QTcpSocket*> clientserverMap;
    bool isValid(QString&);
    void freeconnections();
signals:
    
public slots:
    void newConnection();

private:
    QTcpServer *server;
#define server_name "BigOne"
};

#endif // MYTCPSERVER_H
