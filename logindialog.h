#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "usermanager.h"
#include "databasemanager.h"


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    // بياخد UserManager عشان يتحقق من المستخدمين المسجلين
   explicit LoginDialog(UserManager* userManager, DatabaseManager* dbManager, QWidget* parent = nullptr);
    UserRole getLoggedInUserRole() const { return loggedInUserRole_; }
    QString getLoggedInUserName() const { return loggedInUserName_; }
    ~LoginDialog();

private slots:
    void onLoginClicked();
    void onReturnPressed();

private:
     Ui::LoginDialog* ui;
    DatabaseManager* dbManager_;
    UserManager*     userManager_;
    UserRole loggedInUserRole_;
     QString loggedInUserName_;

};

#endif // LOGINDIALOG_H