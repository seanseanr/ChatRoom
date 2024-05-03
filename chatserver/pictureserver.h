#ifndef PICTURESERVER_H
#define PICTURESERVER_H

#include <QTcpServer>
#include <QImage>

class ChatServer;

class pictureserver : public QTcpServer
{
    Q_OBJECT
public:
    explicit pictureserver(QObject *parent = 0);
    ~pictureserver();
    void add_username(QString &s){pendingUserName = s;}
    void dispatchPic(QByteArray &ba);
    qint64 expected_bytes;
    qint64 written_size;
private:
    QImage image;
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> users;
    QString pendingUserName;
    QByteArray ba;
    int tmp_idx = 0;

protected:
    void incomingConnection(int socketfd);

signals:
    
private slots:
    void readyRead();
    void disconnected();
    
};

#endif // PICTURESERVER_H
