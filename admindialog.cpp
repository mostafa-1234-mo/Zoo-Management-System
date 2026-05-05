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

    setWindowTitle("Admin Login");
    setFixedSize(360, 220);

    // Format error message
    ui->labelAdminError->setStyleSheet(
        "color: #a32d2d; font-size: 12px;"
        );
    ui->labelAdminError->hide();

    /* * 💡 The old buttonBox code was removed because it was causing the issue.
     * If you added a "Login" button in the UI and named it e.g. btnLogin,
     * uncomment the line below to make it work:
     */
    // connect(ui->btnLogin, &QPushButton::clicked, this, &AdminDialog::onLoginClicked);

    // Connect the Enter key from the keyboard (so it logs in immediately when pressing enter after typing the password)
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
            "Please enter the admin password!");
        ui->labelAdminError->show();
        return;
    }

    if (userManager_->checkAdminPassword(password)) {
        // ── Log admin login ──────────────────
        LOG_INFO("Admin logged in successfully");
        accept();
    } else {
        // ── Log failed login attempt ───────────────
        LOG_WARN("Failed admin login attempt — "
                 "Incorrect password");

        ui->labelAdminError->setText(
            "Incorrect admin password!");
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