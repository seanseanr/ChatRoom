#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupFilemenu();
    setupEditor();
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
    MyHighlighter myhighlighter(editor->document());
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
#define server_name "BigOne: "
    editor->append(server_name + lineditor->text());
}

void MainWindow::newFile()
{

}

void MainWindow::saveFile()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}
