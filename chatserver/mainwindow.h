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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void newFile();
    void saveFile();
    void newMeDisplay();

private:
    Ui::MainWindow *ui;
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
};

#endif // MAINWINDOW_H
