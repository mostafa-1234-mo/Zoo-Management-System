#include "logindialog.h"
#include "ui_logindialog.h"
#include "logger.h"
#include "mainwindow.h"


// ── Constructor ────────────────────────────────────────────────
LoginDialog::LoginDialog(UserManager* userManager, DatabaseManager* dbManager, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , dbManager_(dbManager)
    , userManager_(userManager)

{
    ui->setupUi(this);

    setWindowTitle("تسجيل الدخول");
    setFixedSize(380, 260);

    setWindowFlag(Qt::WindowCloseButtonHint, false);

    ui->labelError->setStyleSheet(
        "color: #a32d2d; font-size: 12px;"
        );
    ui->labelError->hide();

    connect(ui->btnLogin,
            &QPushButton::clicked,
            this,
            &LoginDialog::onLoginClicked);

    connect(ui->lineEditUsername,
            &QLineEdit::returnPressed,
            this,
            &LoginDialog::onReturnPressed);

    connect(ui->lineEditPassword,
            &QLineEdit::returnPressed,
            this,
            &LoginDialog::onReturnPressed);

    ui->lineEditUsername->setFocus();
}

// ── Destructor ─────────────────────────────────────────────────
LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onLoginClicked()
{
    // 1. صمام أمان للمديرين
    if (userManager_ == nullptr || dbManager_ == nullptr) {
        LOG_ERROR("خطأ: مؤشر المانجر أو الداتابيز مفقود!");
        return;
    }

    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->labelError->setText("من فضلك أدخل اسم المستخدم وكلمة السر!");
        ui->labelError->show();
        return;
    }

    // محاولة تسجيل الدخول
    const User* user = userManager_->loginUser(username, password);

    if (user != nullptr) {
        LOG_INFO("تسجيل دخول ناجح للمستخدم: " + username);

        // 2. تخزين البيانات في متغيرات الكلاس (عشان الـ main يوصلها)
        this->loggedInUserRole_ = user->getRole();
        this->loggedInUserName_ = user->getUsername();

        // 3. ✅ الخطوة الأهم: وافق واقفل الشاشة
        // مش هنفتح MainWindow هنا، الـ main.cpp هو اللي هيفتحها
        accept();

    } else {
        LOG_WARN("محاولة دخول فاشلة للمستخدم: " + username);
        ui->labelError->setText("اسم المستخدم أو كلمة السر غلط!");
        ui->labelError->show();
        ui->lineEditPassword->clear();
        ui->lineEditPassword->setFocus();
    }
}

// ── onReturnPressed ────────────────────────────────────────────
void LoginDialog::onReturnPressed()
{
    onLoginClicked();
}