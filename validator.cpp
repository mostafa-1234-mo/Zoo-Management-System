#include "validator.h"
#include <QRegularExpression>

// ── validateName ───────────────────────────────────────────────
ValidationResult Validator::validateName(const QString& name)
{
    QString trimmed = name.trimmed();

    if (trimmed.isEmpty())
        return ValidationResult::fail("Name cannot be empty!");

    if (trimmed.length() < 2)
        return ValidationResult::fail(
            "Name must be at least 2 characters long!");

    if (trimmed.length() > 50)
        return ValidationResult::fail(
            "Name cannot exceed 50 characters!");

    // Check that the name doesn't contain numbers or symbols
    // \p{L} means any letter from any language
    // \s means space
    QRegularExpression nameRegex(R"(^[\p{L}\s]+$)");
    if (!nameRegex.match(trimmed).hasMatch())
        return ValidationResult::fail(
            "Name must not contain numbers or symbols!");

    return ValidationResult::ok();
}

// ── validateUsername ───────────────────────────────────────────
ValidationResult Validator::validateUsername(
    const QString& username)
{
    if (username.isEmpty())
        return ValidationResult::fail(
            "Username cannot be empty!");

    if (username.length() < 4)
        return ValidationResult::fail(
            "Username must be at least 4 characters long!");

    if (username.length() > 20)
        return ValidationResult::fail(
            "Username cannot exceed 20 characters!");

    // Only English letters (lower/upper), numbers, and underscore
    QRegularExpression usernameRegex(R"(^[a-zA-Z0-9_]+$)");
    if (!usernameRegex.match(username).hasMatch())
        return ValidationResult::fail(
            "Username must contain only English letters, "
            "numbers, and _!");

    // Must not start with a number
    if (username[0].isDigit())
        return ValidationResult::fail(
            "Username must not start with a number!");

    return ValidationResult::ok();
}

// ── validatePassword ───────────────────────────────────────────
ValidationResult Validator::validatePassword(
    const QString& password)
{
    if (password.isEmpty())
        return ValidationResult::fail(
            "Password cannot be empty!");

    if (password.length() < 6)
        return ValidationResult::fail(
            "Password must be at least 6 characters long!");

    if (password.length() > 50)
        return ValidationResult::fail(
            "Password cannot exceed 50 characters!");

    // Check for at least one uppercase letter
    bool hasUpper = false;
    // Check for at least one lowercase letter
    bool hasLower = false;
    // Check for at least one digit
    bool hasDigit = false;

    for (const QChar& c : password) {
        if (c.isUpper()) hasUpper = true;
        if (c.isLower()) hasLower = true;
        if (c.isDigit()) hasDigit = true;
    }

    if (!hasUpper)
        return ValidationResult::fail(
            "Password must contain at least one uppercase letter!");

    if (!hasLower)
        return ValidationResult::fail(
            "Password must contain at least one lowercase letter!");

    if (!hasDigit)
        return ValidationResult::fail(
            "Password must contain at least one digit!");

    return ValidationResult::ok();
}

// ── validatePasswordConfirm ────────────────────────────────────
ValidationResult Validator::validatePasswordConfirm(
    const QString& password,
    const QString& confirm)
{
    if (confirm.isEmpty())
        return ValidationResult::fail(
            "Please confirm the password!");

    if (password != confirm)
        return ValidationResult::fail(
            "Passwords do not match!");

    return ValidationResult::ok();
}

// ── validateSpecies ────────────────────────────────────────────
ValidationResult Validator::validateSpecies(
    const QString& species)
{
    QString trimmed = species.trimmed();

    if (trimmed.isEmpty())
        return ValidationResult::fail(
            "Animal species cannot be empty!");

    if (trimmed.length() < 2)
        return ValidationResult::fail(
            "Animal species must be at least 2 characters long!");

    if (trimmed.length() > 30)
        return ValidationResult::fail(
            "Animal species cannot exceed 30 characters!");

    // Letters and spaces only
    QRegularExpression speciesRegex(R"(^[\p{L}\s]+$)");
    if (!speciesRegex.match(trimmed).hasMatch())
        return ValidationResult::fail(
            "Animal species must contain only letters!");

    return ValidationResult::ok();
}

// ── validateFood ───────────────────────────────────────────────
ValidationResult Validator::validateFood(const QString& food)
{
    QString trimmed = food.trimmed();

    if (trimmed.isEmpty())
        return ValidationResult::fail(
            "Food type cannot be empty!");

    if (trimmed.length() < 3)
        return ValidationResult::fail(
            "Food description must be at least 3 characters long!");

    if (trimmed.length() > 100)
        return ValidationResult::fail(
            "Food description cannot exceed 100 characters!");

    return ValidationResult::ok();
}

// ── validateAge ────────────────────────────────────────────────
ValidationResult Validator::validateAge(int age)
{
    if (age < 0)
        return ValidationResult::fail(
            "Age cannot be less than zero!");

    if (age > 100)
        return ValidationResult::fail(
            "Age cannot exceed 100 years!");

    return ValidationResult::ok();
}

// ── validateQuantity ───────────────────────────────────────────
ValidationResult Validator::validateQuantity(int qty)
{
    if (qty < 1)
        return ValidationResult::fail(
            "Quantity must be at least 1!");

    if (qty > 20)
        return ValidationResult::fail(
            "Quantity cannot exceed 20 tickets!");

    return ValidationResult::ok();
}

// ── applyStyle ─────────────────────────────────────────────────
void Validator::applyStyle(QLineEdit* field, bool isValid)
{
    if (isValid) {
        // Light green when data is valid
        field->setStyleSheet(
            "QLineEdit {"
            "  border: 1.5px solid #639922;"
            "  border-radius: 6px;"
            "  padding: 7px 10px;"
            "  background: #f6fbf0;"
            "}"
            );
    } else {
        // Light red when data is invalid
        field->setStyleSheet(
            "QLineEdit {"
            "  border: 1.5px solid #e24b4a;"
            "  border-radius: 6px;"
            "  padding: 7px 10px;"
            "  background: #fff8f8;"
            "}"
            );
    }
}

// ── showError ──────────────────────────────────────────────────
void Validator::showError(QLabel* label, const QString& message)
{
    label->setText("⚠ " + message);
    label->setStyleSheet(
        "color: #a32d2d;"
        "font-size: 12px;"
        "background: #fcebeb;"
        "border-radius: 4px;"
        "padding: 4px 8px;"
        );
    label->show();
}

// ── hideError ──────────────────────────────────────────────────
void Validator::hideError(QLabel* label)
{
    label->hide();
    label->clear();
}