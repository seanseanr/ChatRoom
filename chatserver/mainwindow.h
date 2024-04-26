#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <myhighlighter.h>
#include <QVBoxLayout>
#include <QLineEdit>
#include "chatserver.h"
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
#define server_name "BigOne"

private slots:
    void newFile();
    void saveFile();
    void newMeDisplay();
    void readyRead();
    void connected();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    void setupEditor();
    void setupFilemenu();
    void setupServer();
    QString fileName;
    QTextEdit *editor;
    QLineEdit *lineditor;
    QVBoxLayout *vblayout;
    QMenu *menu;
    QMenuBar *menuBar;
    enum FileMenuAction {
        _NEWACTION,
        _SAVEACTION,
        _TOTALACTIONS
    };
    QAction *act[_TOTALACTIONS];
    ChatServer *server;
};

#endif // MAINWINDOW_H
