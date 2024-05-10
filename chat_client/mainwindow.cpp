#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include "logindialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCodec>
#include <QTextEncoder>

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
    loginDialog->close();
    #endif
    server = new ChatServer(this);
    setupFilemenu();
    setupEditor();
#ifdef CHAT_CLIENT
    setWindowTitle(username);
#else
    setWindowTitle(server_name);
#endif
    setupPic();
}

void MainWindow::connected()
{
    //if(!loggined)
    //{
        QTextCodec *c = QTextCodec::codecForName("UTF-8");
        QTextEncoder *codec = c->makeEncoder(QTextCodec::IgnoreHeader);
        QByteArray sendData = codec->fromUnicode(LOGIN_SIGN + username);

        if(socket->write(sendData) >=0)
        {
            socket->waitForBytesWritten();
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error has occured !");
            messageBox.setFixedSize(500,200);
            messageBox.show();
        }
    //}
}

void MainWindow::readyRead()
{
    if(waitforpic)
    {
        do
        {
            written_bytes += socket->bytesToWrite();
            ar.append(socket->readAll());
        }while(socket->bytesAvailable());
        if(written_bytes >= expected_bytes)
        {
            QFile img(get_cp_picname());
            img.open(QFile::Truncate | QFile::WriteOnly);
            img.write(ar);
            img.close();
            ar.clear();
            waitforpic = false;
        }
    }
    else
    {
        do
        {
            QString line = QString::fromUtf8(socket->readLine().trimmed());
            editor->append(line);
        }while(socket->canReadLine());
    }
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
        //socket->write(s.toAscii());
        QTextCodec *c = QTextCodec::codecForName("UTF-8");
        QTextEncoder *codec = c->makeEncoder(QTextCodec::IgnoreHeader);
        QByteArray sendData = codec->fromUnicode(lineditor->text());
        socket->write(sendData);
        //socket->write(lineditor->text().toAscii());
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
    QString fileName = QFileDialog::getSaveFileName(0, "Save to File", "D:\\", "HTML File (*.html)");
    QFile file(fileName);
    if(!fileName.isEmpty() && file.open(QFile::Text | QFile::Truncate | QFile::WriteOnly))
    {
        QTextStream out(&file);
        QString text = editor->toHtml();
        out<<text;
    }
}

void MainWindow::getPicName()
{
    QByteArray data;
    QString picName = QFileDialog::getOpenFileName(this, "Select Picture", "D:\\", "Picture File (*.png *.jpg *.bmp)");
    if(picName.isEmpty())
        return;
    QFile file(picName);
    file.open(QFile::ReadOnly);
    data.append(file.readAll());
    QTime dat_time;
    QString tmp_picname = QApplication::applicationDirPath() + "/" + dat_time.currentTime().toString("HH_mm_ss") + QString(picName.data() + picName.lastIndexOf("."));
    //QString tmp_picname = "D:/server_tmptmp.png";
    set_cp_picname(tmp_picname);
    QString newFileName = QString(picName.data() + picName.lastIndexOf("/") + 1);
    QString newName = QApplication::applicationDirPath() + "/" + newFileName;
#ifdef CHAT_CLIENT
    QTextCodec *c = QTextCodec::codecForName("UTF-8");
    QTextEncoder *codec = c->makeEncoder(QTextCodec::IgnoreHeader);
    QByteArray arr = codec->fromUnicode(QString("<img src=\"%1\" />").arg(get_cp_picname()));
    QString number = QString("%1").arg(file.size());
    //QByteArray pic_ar = codec->fromUnicode(TS_PIC_SIGN + number);
    socket->write(QString(TS_PIC_SIGN + number + '\0').toAscii());
    socket->flush();
    socket->waitForBytesWritten();
    socket->write(data);
    socket->flush();
    socket->waitForBytesWritten();
    waitforpic = true;
    expected_bytes = file.size();
    written_bytes = 0;
    //socket->waitForReadyRead();
    socket->write(arr);
    socket->flush();
    socket->waitForBytesWritten();
    file.close();
#else
    QString s_with_servername = QString(server_name) + ": " +QString("<img src=\"%1\" />").arg(get_cp_picname());
    editor->append(s_with_servername);
    server->dispatchLine(s_with_servername);
    dispatchPic(data);
#endif
    file.close();
}

void MainWindow::setupPic()
{
    act[_OPENACTION] = new QAction("Select Picture", this);
    connect(act[_OPENACTION], SIGNAL(triggered()) , this, SLOT(getPicName()));
    menu->addAction(act[_OPENACTION]);
    waitforpic = false;
    //editor->append("<img src=copy.png /img>");
}

void MainWindow::appendEditorFromclient(QString s)
{
    editor->append(s);
}

MainWindow::~MainWindow()
{

    delete ui;
}
