#include "logindialog.h"
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{
    setupGUI();
    setWindowTitle("User Login");
    setModal(true);
}

void LoginDialog::setupGUI()
{
    QGridLayout *gridLayout = new QGridLayout(this);
    lineUserName = new QLineEdit(this);
    labelUserName = new QLabel(this);
    labelUserName->setText("User Name:");
    buttons = new QDialogButtonBox(this);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText( tr("Login") );
    buttons->button(QDialogButtonBox::Cancel)->setText( tr("Abort"));
    connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(rejectLoginSlot()));
    connect( buttons->button( QDialogButtonBox::Ok ),SIGNAL (clicked()), this, SLOT (acceptLoginSlot()));
    connect( this ,SIGNAL(accepted()), this, SLOT(close()) );
    connect( this ,SIGNAL(rejected()), this, SLOT(close()) );

    gridLayout->addWidget(labelUserName, 0, 0);
    gridLayout->addWidget(lineUserName, 0, 1);
    gridLayout->addWidget(buttons, 2, 0, 1, 2);

    setLayout(gridLayout);
    setWindowTitle("User Login");

    //emit acceptLogin(username);
}

void LoginDialog::rejectLoginSlot()
{
    username.clear();
    emit rejected();
}

void LoginDialog::acceptLoginSlot()
{
    username = lineUserName->text();

    if(!username.isEmpty())
        emit accepted();
    //return UserName;
}

void LoginDialog::setUserName()
{
    username = lineUserName->text();
}
