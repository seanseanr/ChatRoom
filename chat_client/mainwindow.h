#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include "myhighlighter.h"
#include "chatserver.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextCodec>
#include <QTextEncoder>
#include <QLabel>
#include <QList>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loggined = false;
    bool waitforpic = false;
    QString username;
    void appendEditorFromclient(QString s);
    ChatServer *server;
    qint64 expected_bytes;
    qint64 written_bytes;
    QString get_cp_picname(){return cp_picname;}
    void set_cp_picname(QString s){cp_picname = s;}
    void qt_wait_ms(float amount);
#define server_name "BigOne"

private slots:
    void newFile();
    void saveFile();
    void newMeDisplay();
    void newUserHighLight();
    //void transferPic();
    //void receivePic();
    void getPicName();
    void readyRead();
    void connected();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket, *picsocket;
    void setupEditor();
    void setupFilemenu();
    void setupServer();
    void setupPic();
    QString fileName;
    QString tailed_s;
    int tmp_idx = 0;
    QString cp_picname;
    //QString PicName;
    MyHighlighter *highlighter;
    QTextEdit *editor;
    QLineEdit *lineditor;
    QLineEdit *lineditor2;
    QLabel *lbl_msg;
    QLabel *lbl_user;
    QVBoxLayout *vblayout;
    QMenu *menu;
    QMenuBar *menuBar;
    enum FileMenuAction {
        _NEWACTION,
        _SAVEACTION,
        _OPENACTION,
        _TOTALACTIONS
    };
    QAction *act[_TOTALACTIONS];
    QByteArray ar;
    QByteArray arr;
    QString picName;
    QTextCodec *c;
    QTextEncoder *codec;
    QByteArray tokdat;
};

#endif // MAINWINDOW_H
