#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = 0);
    QString username;

private:
    QLabel *labelUserName;
    QLineEdit *lineUserName;
    QDialogButtonBox *buttons;
    void setupGUI();
    void setUserName();

signals:
    //void acceptLogin(QString & UserName);
public slots:
    void acceptLoginSlot();
    void rejectLoginSlot();
};

#endif // LOGINDIALOG_H
