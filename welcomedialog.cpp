#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "registerdialog.h"
#include "logindialog.h"
#include "admindialog.h"

// ── Constructor ────────────────────────────────────────────────
WelcomeDialog::WelcomeDialog(UserManager* userManager, DatabaseManager* dbManager, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::WelcomeDialog)
    , dbManager_(dbManager)
    , userManager_(userManager)
{
   ui->setupUi(this);

    setWindowTitle("أهلاً بك");
    setFixedSize(400, 300);

    // مش هيظهر زرار الإغلاق
    setWindowFlag(Qt::WindowCloseButtonHint, false);

    // تنسيق الزراين
    ui->btnRegister->setStyleSheet(
        "QPushButton {"
        "  background: #E6F1FB;"
        "  color: #0C447C;"
        "  border: 0.5px solid #85B7EB;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: #B5D4F4; }"
        );

    ui->btnAdminLogin->setStyleSheet(
        "QPushButton {"
        "  background: #EEEDFE;"
        "  color: #3C3489;"
        "  border: 0.5px solid #AFA9EC;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: #CECBF6; }"
        );
    ui->btnLogin->setStyleSheet(
        "QPushButton {"
        "  background: #EAF3DE;"
        "  color: #27500A;"
        "  border: 0.5px solid #97C459;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: #C0DD97; }"
        );

    connect(ui->btnRegister,
            &QPushButton::clicked,
            this,
            &WelcomeDialog::onRegisterClicked);

    connect(ui->btnAdminLogin,
            &QPushButton::clicked,
            this,
            &WelcomeDialog::onAdminLoginClicked);

    connect(ui->btnLogin,
            &QPushButton::clicked,
            this,
            &WelcomeDialog::onLoginClicked);



}

// ── Destructor ─────────────────────────────────────────────────
WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

// ── getResult ──────────────────────────────────────────────────
WelcomeResult WelcomeDialog::getResult() const
{
    return result_;
}

// ── onRegisterClicked ──────────────────────────────────────────
void WelcomeDialog::onRegisterClicked()
{
    // الخطوة 1: شاشة إنشاء الحساب
    RegisterDialog regDialog(userManager_, this);

    if (regDialog.exec() != QDialog::Accepted) {
        return; // المستخدم ضغط إلغاء — ارجع للشاشة الأولى
    }

    // الخطوة 2: بعد ما أنشأ الحساب، شاشة تسجيل الدخول
    LoginDialog loginDialog(userManager_, dbManager_, this);

    if (loginDialog.exec() != QDialog::Accepted) {
        return; // المستخدم ضغط إلغاء
    }

    // نجح الدخول
    result_ = WelcomeResult::REGISTER_THEN_LOGIN;
    accept(); // اقفل الـ WelcomeDialog بـ Accepted
}

// ── onAdminLoginClicked ────────────────────────────────────────
void WelcomeDialog::onAdminLoginClicked()
{
    // شاشة كلمة سر المدير
    AdminDialog adminDialog(userManager_, this);

    if (adminDialog.exec() != QDialog::Accepted) {
        return; // المستخدم ضغط إلغاء
    }

    // نجح دخول المدير
    result_ = WelcomeResult::ADMIN_LOGIN;
    accept();
}
// ── onLoginClicked ─────────────────────────────────────────────
void WelcomeDialog::onLoginClicked()
{
    // مباشرة شاشة تسجيل الدخول من غير تسجيل
   LoginDialog loginDialog(userManager_, dbManager_, this);

    if (loginDialog.exec() != QDialog::Accepted) {
        return; // المستخدم ضغط إلغاء — ارجع للشاشة الأولى
    }

    // نجح الدخول
    result_ = WelcomeResult::REGISTER_THEN_LOGIN;
    accept();
}