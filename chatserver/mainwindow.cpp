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
    loginDialog->close();
    #else
    server = new ChatServer(this);
    picserver = new pictureserver(this);
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

void MainWindow::Picconnected()
{

}

void MainWindow::PicreadyRead()
{
    do
    {
        written_bytes += picsocket->bytesAvailable();
        ar.append(picsocket->readAll());

    }while(picsocket->bytesAvailable());

    if(written_bytes >= expected_bytes)
    {
        QImage img("client_tmp.png");
        img.fromData(ar);
        img.save("client_tmp.png");
        ar.clear();
    }
}

void MainWindow::connected()
{
    //if(!loggined)
    //{
        if(socket->write(QString(LOGIN_SIGN + username).toUtf8()) >=0)
        {
            socket->waitForBytesWritten();
            //loggined = true;
            picsocket = new QTcpSocket(this);
            connect(picsocket, SIGNAL(readyRead()), this, SLOT(PicreadyRead()));
            connect(picsocket, SIGNAL(connected()), this, SLOT(Picconnected()));
            picsocket->connectToHost(QHostAddress::LocalHost, 5050);

        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error has occured !");
            messageBox.setFixedSize(500,200);
        }
    //}
}

void MainWindow::readyRead()
{
    do
    {
        QString line = QString::fromUtf8(socket->readLine().trimmed());
        editor->append(line);
        //QByteArray arr = socket->readAll();
        //QString tmpfileName = QApplication::applicationDirPath() + "/" + QString("tmp%1.png").arg(++tmp_idx);
        //QFile file(tmpfileName);
        //editor->append(QString("<img src=\"%1\"/>").arg(tmpfileName));
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
        //socket->write(s.toUtf8());
        socket->write(lineditor->text().toUtf8());
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

void MainWindow::getPicName()
{
    QByteArray data;
    QString picName = QFileDialog::getOpenFileName(this, "Select Picture", "D:\\", "Picture File (*.png *.jpg *.bmp)");
    if(picName.isEmpty())
        return;
    //QFile file(picName);
    //file.open(QFile::ReadOnly);
    //data.append(file.readAll().data());
    QString newFileName = QString(picName.data() + picName.lastIndexOf("/") + 1);
    QString newName = QApplication::applicationDirPath() + "/" + newFileName;
#ifdef CHAT_CLIENT
    editor->append(username + ": ");
    editor->append(QString("<img src=\"%1\" />").arg(picName));
    socket->write(QString("<img src=\"%1\" />").arg(picName).toUtf8());
#else
    QString s_with_servername = QString(server_name) + ": " +QString("<img src=\"%1\" />").arg(picName);
    editor->append(s_with_servername);
    server->dispatchLine(s_with_servername);
#endif
    //picserver->expected_bytes = file.size();
    //picserver->written_size = 0;
    //picsocket->write(data);
    //picsocket->flush();
    //picsocket.close();
    //picsocket->waitForBytesWritten();
}

void MainWindow::setupPic()
{
    act[_OPENACTION] = new QAction("Select Picture", this);
    connect(act[_OPENACTION], SIGNAL(triggered()) , this, SLOT(getPicName()));
    menu->addAction(act[_OPENACTION]);
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
