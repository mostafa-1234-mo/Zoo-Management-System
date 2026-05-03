#include "admindialog.h"
#include "ui_admindialog.h"
#include <QPushButton>
#include "logger.h"

// ── Constructor ────────────────────────────────────────────────
AdminDialog::AdminDialog(UserManager* userManager,
                         QWidget* parent)
    : QDialog(parent),
    ui(new Ui::AdminDialog),
    userManager_(userManager)
{
    ui->setupUi(this);

    setWindowTitle("دخول المدير");
    setFixedSize(360, 220);

    // تنسيق رسالة الخطأ
    ui->labelAdminError->setStyleSheet(
        "color: #a32d2d; font-size: 12px;"
        );
    ui->labelAdminError->hide();

    /* * 💡 تم مسح كود الـ buttonBox القديم لأنه كان بيعمل المشكلة.
     * لو إنت ضفت زرار "دخول" في التصميم وسميته مثلا btnLogin،
     * شيل الشرطتين (//) من السطر اللي تحت عشان يشتغل:
     */
    // connect(ui->btnLogin, &QPushButton::clicked, this, &AdminDialog::onLoginClicked);

    // ربط زرار Enter من الكيبورد (عشان أول ما يكتب الباسورد ويدوس انتر يدخل على طول)
    connect(ui->lineEditAdminPass,
            &QLineEdit::returnPressed,
            this,
            &AdminDialog::onReturnPressed);

    ui->lineEditAdminPass->setFocus();
}

// ── Destructor ─────────────────────────────────────────────────
AdminDialog::~AdminDialog()
{
    delete ui;
}

// ── onLoginClicked ─────────────────────────────────────────────
void AdminDialog::onLoginClicked()
{
    QString password = ui->lineEditAdminPass->text();

    if (password.isEmpty()) {
        ui->labelAdminError->setText(
            "من فضلك أدخل كلمة سر المدير!");
        ui->labelAdminError->show();
        return;
    }

    if (userManager_->checkAdminPassword(password)) {
        // ── سجّل دخول المدير ──────────────────
        LOG_INFO("تسجيل دخول المدير بنجاح");
        accept();
    } else {
        // ── سجّل محاولة الاختراق ───────────────
        LOG_WARN("محاولة دخول فاشلة كمدير — "
                 "كلمة السر غلط");

        ui->labelAdminError->setText(
            "كلمة سر المدير غلط!");
        ui->labelAdminError->show();
        ui->lineEditAdminPass->clear();
        ui->lineEditAdminPass->setFocus();
    }
}

// ── onReturnPressed ────────────────────────────────────────────
void AdminDialog::onReturnPressed()
{
    onLoginClicked();
}