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
#define TS_PIC_SIGN "PIC1234:"
#define TS_PIC_SIGN_LEN sizeof(TS_PIC_SIGN)
    ~ChatServer();
    void dispatchLine(QString line);
    void qt_wait_ms(float amount);
    
protected:
    void incomingConnection(int socketfd);
    friend class MainWindow;
    friend class pictureserver;

private:
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*, QPair<QString, bool> > users;
    QByteArray ba;
    QString num_s;
    qint64 expected_bytes;
    qint64 written_size;
    void dispatchPic(QByteArray ba);
    QByteArray tokdat;

signals:
    void message_signal(QString msg);

private slots:
    void readyRead();
    void disconnected();
};

#endif // CHATSERVER_H
