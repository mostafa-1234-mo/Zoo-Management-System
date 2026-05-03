#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include "usermanager.h"

namespace Ui {
class WelcomeDialog;
}

// ── نتيجة الـ WelcomeDialog ────────────────────
// بنحتاجها في main.cpp نعرف المستخدم اختار إيه
enum class WelcomeResult {
    REGISTER_THEN_LOGIN, // اختار إنشاء حساب
    ADMIN_LOGIN,         // اختار دخول المدير
    CANCELLED            // أغلق البرنامج
};

class WelcomeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeDialog(UserManager* userManager, DatabaseManager* dbManager, QWidget* parent = nullptr);

    QString getLoggedInUsername() const { return loggedInUsername_; }
    ~WelcomeDialog();

    // بترجع اختيار المستخدم
    WelcomeResult getResult() const;

private slots:
    void onRegisterClicked();
    void onAdminLoginClicked();
        void onLoginClicked();

private:
     Ui::WelcomeDialog* ui;
    QString loggedInUsername_;
    DatabaseManager* dbManager_;
    UserManager*       userManager_;
    WelcomeResult      result_;
};

#endif // WELCOMEDIALOG_H