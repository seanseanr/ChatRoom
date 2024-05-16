#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include "logindialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCodec>
#include <QTextEncoder>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QTimer>
#include <QEventLoop>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef CHAT_CLIENT
    LoginDialog *loginDialog = new LoginDialog(this);
    loginDialog->setModal(true);
    loginDialog->exec();
    if(!loginDialog->username.isEmpty())
    {
        username = loginDialog->username;
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(socket, SIGNAL(connected()), this, SLOT(connected()));
        socket->connectToHost("192.168.90.79", 5000);
    }
    else
    {
        username.clear();
    }
    loginDialog->close();
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
            written_bytes += socket->bytesAvailable();
            ar.append(socket->readAll());
        }while(socket->bytesAvailable());
        if(written_bytes >= expected_bytes)
        {
            QTime time;
            QString tmp_picname = QApplication::applicationDirPath() + "/" + time.currentTime().toString("HH_mm_ss_zzz") + QString(picName.data() + picName.lastIndexOf("."));
            //QTextCodec *c = QTextCodec::codecForName("UTF-8");
            //QTextEncoder *codec = c->makeEncoder(QTextCodec::IgnoreHeader);
            //arr = codec->fromUnicode(tmp_picname);
            set_cp_picname(tmp_picname);
            waitforpic = false;
            QString tmp_tag = QString("<img src=\"%1\" />").arg(get_cp_picname());
            QFile img(get_cp_picname());
            img.open(QFile::Truncate | QFile::WriteOnly);
            img.write(ar);
            img.close();
            editor->append(tmp_tag);
            //qt_wait_ms(0.5);

            return;
        }
    }
    else
    {
        QString line = QString::fromUtf8(socket->readLine().trimmed());
        if(line.contains(TS_PIC_SIGN))
        {
            tailed_s = QString(line.data() + TS_PIC_SIGN_LEN - 1);
            waitforpic = true;
            expected_bytes = tailed_s.toInt();
            written_bytes = 0;
            return;
        }
        editor->append(line);
        while(socket->canReadLine())
        {
            editor->append(line);
        }
    }
}

void MainWindow::setupEditor()
{
    QWidget *centralWidget = new QWidget();
    editor = new QTextEdit;
    lineditor = new QLineEdit;
    lineditor2 = new QLineEdit;
    lbl_msg = new QLabel("Enter message:", this);
    lbl_user = new QLabel("Enter user to highlight:", this);
    editor->setFont(QFont("courier"));
    editor->setFontPointSize(10);
    editor->setReadOnly(true);
    editor->setFixedHeight(175);
#ifdef CHAT_CLIENT
    highlighter = new MyHighlighter(editor->document(), username, "");
#else
    highlighter = new MyHighlighter(editor->document(), server_name, "");
#endif
    lineditor->setFont(QFont("courier"));
    lineditor->setMinimumSize(10, 10);
    lineditor->setFixedHeight(25);
    lineditor->setReadOnly(false);
    vblayout = new QVBoxLayout(centralWidget);

    vblayout->addWidget(editor);
    vblayout->addWidget(lbl_msg);
    vblayout->addWidget(lineditor);
    vblayout->addWidget(lbl_user);
    vblayout->addWidget(lineditor2);
    setCentralWidget(centralWidget);
    connect(lineditor, SIGNAL(editingFinished()), this, SLOT(newMeDisplay()));
    connect(lineditor2, SIGNAL(editingFinished()), this, SLOT(newUserHighLight()));
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
        QTextCodec *c = QTextCodec::codecForName("UTF-8");
        QTextEncoder *codec = c->makeEncoder(QTextCodec::IgnoreHeader);
        QByteArray sendData = codec->fromUnicode(lineditor->text());
        socket->write(sendData);
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

void MainWindow::newUserHighLight()
{
    if(!lineditor2->text().isEmpty())
    {
        if(highlighter && (lineditor2->text().trimmed()!=username))
        {
            delete highlighter;
            highlighter = new MyHighlighter(editor->document(), username, lineditor2->text().trimmed());
        }
    }
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
    if(waitforpic)
    {
        return;
    }
    picName = QFileDialog::getOpenFileName(this, "Select Picture", "D:\\", "Picture File (*.png *.jpg *.bmp)");
    if(picName.isEmpty())
        return;
    QFile file(picName);
    file.open(QFile::ReadOnly);
    data.append(file.readAll());
#ifdef CHAT_CLIENT
    QString number = QString("%1").arg(file.size());
    ar.clear();
    socket->write(QString(TS_PIC_SIGN + number + '\0').toAscii());
    socket->flush();
    socket->waitForBytesWritten();
    qt_wait_ms(0.01);
    socket->write(data);
    socket->flush();
    socket->waitForBytesWritten();
    //qt_wait_ms(0.01);
#else
    QString s_with_servername = QString(server_name) + ": " +QString("<img src=\"%1\" />").arg(get_cp_picname());
    editor->append(s_with_servername);
    server->dispatchLine(s_with_servername);
    dispatchPic(data);
#endif
    file.close();
}

void MainWindow::qt_wait_ms(float amount)
{
#if 1
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<amount*1000)
    {
        //QCoreApplication::processEvents();
    }
#else
    QTimer timer;
    //        timer.setSingleShot(true);
    timer.setInterval(amount*1);
    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start();
    loop.exec();
#endif
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
