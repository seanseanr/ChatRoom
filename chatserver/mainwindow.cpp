#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <logindialog.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    #ifdef CHAT_CLIENT
    LoginDialog *loginDialog = new LoginDialog(this);
    loginDialog->setModal(true);
    loginDialog->exec();
    //loginDialog->show();
    //if(loginDialog->username.isEmpty())
    if(!loginDialog->username.isEmpty())
    {
        username = loginDialog->username;
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(socket, SIGNAL(connected()), this, SLOT(connected()));
        socket->connectToHost(QHostAddress::LocalHost, 5000);
    }
    else
    {
        username.clear();
    }
    #else
    server = new ChatServer(this);
    #endif
    setupFilemenu();
    setupEditor();
#ifdef CHAT_CLIENT
    setWindowTitle(username);
#else
    setWindowTitle(server_name);
#endif
}

void MainWindow::connected()
{
    if(!loggined)
    {
        if(socket->write(QString(LOGIN_SIGN + username).toUtf8()) >=0)
        {
            loggined = true;
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error has occured !");
            messageBox.setFixedSize(500,200);
        }
    }
}

void MainWindow::readyRead()
{
    do
    {
        QString line = QString::fromUtf8(socket->readLine().trimmed());
        editor->append(line);
        //newMeDisplay();
    }while(socket->canReadLine());
}

void MainWindow::setupEditor()
{
    QWidget *centralWidget = new QWidget();
    editor = new QTextEdit;
    lineditor = new QLineEdit;
    editor->setFont(QFont("courier"));
    editor->setFontPointSize(10);
    editor->setReadOnly(true);
    editor->setFixedHeight(175);
#ifdef CHAT_CLIENT
    MyHighlighter *highlighter = new MyHighlighter(editor->document(), username);
#else
    MyHighlighter *highlighter = new MyHighlighter(editor->document(), server_name);
#endif
    lineditor->setFont(QFont("courier"));
    lineditor->setMinimumSize(10, 10);
    lineditor->setFixedHeight(25);
    lineditor->setReadOnly(false);
    vblayout = new QVBoxLayout(centralWidget);

    vblayout->addWidget(editor);
    vblayout->addWidget(lineditor);
    setCentralWidget(centralWidget);
    connect(lineditor, SIGNAL(editingFinished()), this, SLOT(newMeDisplay()));
}

void MainWindow::setupFilemenu()
{
    menu = new QMenu("File");
    act[_NEWACTION] = new QAction("NEW", this);
    act[_SAVEACTION] = new QAction("Save", this);
    menu->addAction(act[_NEWACTION]);
    menu->addAction(act[_SAVEACTION]);
    menuBar = new QMenuBar(this);
    menuBar->addMenu(menu);
    connect(act[_NEWACTION], SIGNAL(triggered()), this, SLOT(newFile()));
    connect(act[_SAVEACTION], SIGNAL(triggered()), this, SLOT(saveFile()));

}

void MainWindow::newMeDisplay()
{
#ifdef CHAT_CLIENT
    if(!lineditor->text().isEmpty())
    {
        QString s = username + ": " + lineditor->text();
        editor->append(s);
        socket->write(s.toUtf8());
    }
#else
    if(!lineditor->text().isEmpty())
    {
        QString s = QString(server_name) + ": " + lineditor->text();
        editor->append(s);
        server->dispatchLine(s);
    }
#endif
    lineditor->clear();
}

void MainWindow::newFile()
{
    editor->clear();
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(0, "Save to File", "D:\\", "Text File (*.txt)");
    QFile file(fileName);
    if(!fileName.isEmpty() && file.open(QFile::Text | QFile::Truncate | QFile::WriteOnly))
    {
        QTextStream out(&file);
        QString text = editor->toPlainText();
        out<<text;
    }
}

void MainWindow::appendEditorFromclient(QString s)
{
    editor->append(s);
}

MainWindow::~MainWindow()
{

    delete ui;
}
