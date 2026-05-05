#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QPushButton>
#include "logger.h"

// ── Constructor ────────────────────────────────────────────────
RegisterDialog::RegisterDialog(UserManager* userManager,
                               QWidget* parent)
    : QDialog(parent),
    ui(new Ui::RegisterDialog),
    userManager_(userManager)
{
    ui->setupUi(this);

    setWindowTitle("Create New Account");
    setFixedSize(400, 320);

    // تنسيق رسالة الخطأ
    ui->labelRegError->setStyleSheet(
        "color: #a32d2d; font-size: 12px;"
        );
    ui->labelRegError->hide();
    // ربط زرار Enter في خانة تأكيد كلمة السر بدالة التسجيل
    connect(ui->lineEditConfirm, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(ui->lineEditFullName,
            &QLineEdit::textChanged,
            this,
            &RegisterDialog::onFullNameChanged);

    connect(ui->lineEditUsername,
            &QLineEdit::textChanged,
            this,
            &RegisterDialog::onUsernameChanged);

    connect(ui->lineEditPassword,
            &QLineEdit::textChanged,
            this,
            &RegisterDialog::onPasswordChanged);

    connect(ui->lineEditConfirm,
            &QLineEdit::textChanged,
            this,
            &RegisterDialog::onConfirmChanged);

    /*
     * 💡 تم مسح الـ buttonBox القديم.
     * لو إنت سحبت زرار "تسجيل" بايدك في الـ Designer وسميته btnRegister،
     * شيل علامة التعليق (//) من السطر اللي جاي ده عشان تربطه بالدالة:
     */
    // connect(ui->btnRegister, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);

    // (اختياري) لو عامل زرار لإلغاء وسميته btnCancel:
    // connect(ui->btnCancel, &QPushButton::clicked, this, &RegisterDialog::reject);

}

// ── Destructor ─────────────────────────────────────────────────
RegisterDialog::~RegisterDialog()
{
    delete ui;
}

// ── validateInputs ─────────────────────────────────────────────
QString RegisterDialog::validateInputs() const
{
    // التحقق من الاسم الكامل
    auto nameResult = Validator::validateName(
        ui->lineEditFullName->text());
    if (!nameResult.isValid)
        return nameResult.message;

    // التحقق من اسم المستخدم
    auto usernameResult = Validator::validateUsername(
        ui->lineEditUsername->text());
    if (!usernameResult.isValid)
        return usernameResult.message;

    // التحقق من كلمة السر
    auto passwordResult = Validator::validatePassword(
        ui->lineEditPassword->text());
    if (!passwordResult.isValid)
        return passwordResult.message;

    // التحقق إن كلمتا السر متطابقتين
    auto confirmResult = Validator::validatePasswordConfirm(
        ui->lineEditPassword->text(),
        ui->lineEditConfirm->text());
    if (!confirmResult.isValid)
        return confirmResult.message;

    // التحقق إن الـ username مش موجود
    if (userManager_->usernameExists(
            ui->lineEditUsername->text()))
      return "Username already exists!";

    return ""; // كل حاجة تمام
}
// ── onFullNameChanged ──────────────────────────────────────────
void RegisterDialog::onFullNameChanged(const QString& text)
{
    auto result = Validator::validateName(text);
    Validator::applyStyle(ui->lineEditFullName,
                          result.isValid || text.isEmpty());

    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelRegError,
                             result.message);
    else
        Validator::hideError(ui->labelRegError);
}

// ── onUsernameChanged ──────────────────────────────────────────
void RegisterDialog::onUsernameChanged(const QString& text)
{
    auto result = Validator::validateUsername(text);
    Validator::applyStyle(ui->lineEditUsername,
                          result.isValid || text.isEmpty());

    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelRegError,
                             result.message);
    else
        Validator::hideError(ui->labelRegError);
}

// ── onPasswordChanged ──────────────────────────────────────────
void RegisterDialog::onPasswordChanged(const QString& text)
{
    auto result = Validator::validatePassword(text);
    Validator::applyStyle(ui->lineEditPassword,
                          result.isValid || text.isEmpty());

    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelRegError,
                             result.message);
    else
        Validator::hideError(ui->labelRegError);
}

// ── onConfirmChanged ───────────────────────────────────────────
void RegisterDialog::onConfirmChanged(const QString& text)
{
    auto result = Validator::validatePasswordConfirm(
        ui->lineEditPassword->text(), text);

    Validator::applyStyle(ui->lineEditConfirm,
                          result.isValid || text.isEmpty());

    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelRegError,
                             result.message);
    else
        Validator::hideError(ui->labelRegError);
}
// ── onRegisterClicked ──────────────────────────────────────────
void RegisterDialog::onRegisterClicked()
{
    QString error = validateInputs();

    if (!error.isEmpty()) {
        ui->labelRegError->setText(error);
        ui->labelRegError->show();
        return;
    }

    QString fullName = ui->lineEditFullName->text().trimmed();
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();

    bool success = userManager_->registerUser(
        fullName, username, password);

    if (success) {
        // ── Log account creation ─────────────────
        LOG_INFO("New account created: " + username +
                 " (" + fullName + ")");

        QMessageBox::information(
            this, "Account Created",
            "Your account has been created successfully!\n"
            "Username: " + username
            );
        accept();
    } else {
        LOG_WARN("Failed to create account: " + username +
                 " — Username already exists");

        ui->labelRegError->setText(
            "An error occurred while creating the account!");
        ui->labelRegError->show();
    }
}