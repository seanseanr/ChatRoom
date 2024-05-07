#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>


class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = 0);
#define LOGIN_SIGN "LOGIN1234:"
#define SIGN_LEN sizeof(LOGIN_SIGN)
    ~ChatServer();
    void dispatchLine(QString line);
    
protected:
    void incomingConnection(int socketfd);
    friend class MainWindow;
    friend class pictureserver;

private:
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> users;

signals:
    void message_signal(QString msg);

private slots:
    void readyRead();
    void disconnected();
};

#endif // CHATSERVER_H
