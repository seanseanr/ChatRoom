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
#include "pictureserver.h"
#include <QVBoxLayout>
#include <QLineEdit>
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
    QString username;
    void appendEditorFromclient(QString s);
    ChatServer *server;
    pictureserver *picserver;
    qint64 expected_bytes;
    qint64 written_bytes;
    QString get_cp_picname(){return cp_picname;}
    void set_cp_picname(QString s){cp_picname = s;}
#define server_name "BigOne"

private slots:
    void newFile();
    void saveFile();
    void newMeDisplay();
    //void transferPic();
    //void receivePic();
    void getPicName();
    void readyRead();
    void connected();
    void Picconnected();
    void PicreadyRead();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket, *picsocket;
    void setupEditor();
    void setupFilemenu();
    void setupServer();
    void setupPic();
    QString fileName;
    int tmp_idx = 0;
    QString cp_picname;
    //QString PicName;
    QTextEdit *editor;
    QLineEdit *lineditor;
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
};

#endif // MAINWINDOW_H
