#include <QApplication>
#include "mainwindow.h"
MainWindow *w;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new MainWindow;
    w->show();
#ifdef CHAT_CLIENT
    if(w->username.isEmpty())
    {
        w->close();
    }
    else
    {

    }
#endif
    return a.exec();
}
