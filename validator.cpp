#include "validator.h"
#include <QRegularExpression>

// ── validateName ───────────────────────────────────────────────
ValidationResult Validator::validateName(const QString& name)
{
    QString trimmed = name.trimmed();

    if (trimmed.isEmpty())
        return ValidationResult::fail("الاسم لا يمكن أن يكون فارغاً!");

    if (trimmed.length() < 2)
        return ValidationResult::fail(
            "الاسم لازم يكون حرفين على الأقل!");

    if (trimmed.length() > 50)
        return ValidationResult::fail(
            "الاسم لا يمكن أن يتجاوز 50 حرفاً!");

    // نتحقق إن الاسم مش بيحتوي أرقام أو رموز
    // \p{L} معناها أي حرف من أي لغة (عربي أو إنجليزي)
    // \s معناها مسافة
    QRegularExpression nameRegex(R"(^[\p{L}\s]+$)");
    if (!nameRegex.match(trimmed).hasMatch())
        return ValidationResult::fail(
            "الاسم لا يجب أن يحتوي على أرقام أو رموز!");

    return ValidationResult::ok();
}

// ── validateUsername ───────────────────────────────────────────
ValidationResult Validator::validateUsername(
    const QString& username)
{
    if (username.isEmpty())
        return ValidationResult::fail(
            "اسم المستخدم لا يمكن أن يكون فارغاً!");

    if (username.length() < 4)
        return ValidationResult::fail(
            "اسم المستخدم لازم يكون 4 حروف على الأقل!");

    if (username.length() > 20)
        return ValidationResult::fail(
            "اسم المستخدم لا يمكن أن يتجاوز 20 حرفاً!");

    // حروف إنجليزية صغيرة وكبيرة وأرقام وـ underscore بس
    QRegularExpression usernameRegex(R"(^[a-zA-Z0-9_]+$)");
    if (!usernameRegex.match(username).hasMatch())
        return ValidationResult::fail(
            "اسم المستخدم يجب أن يحتوي على حروف إنجليزية "
            "وأرقام و _ فقط!");

    // مش يبدأ برقم
    if (username[0].isDigit())
        return ValidationResult::fail(
            "اسم المستخدم لا يجب أن يبدأ برقم!");

    return ValidationResult::ok();
}

// ── validatePassword ───────────────────────────────────────────
ValidationResult Validator::validatePassword(
    const QString& password)
{
    if (password.isEmpty())
        return ValidationResult::fail(
            "كلمة السر لا يمكن أن تكون فارغة!");

    if (password.length() < 6)
        return ValidationResult::fail(
            "كلمة السر لازم تكون 6 حروف على الأقل!");

    if (password.length() > 50)
        return ValidationResult::fail(
            "كلمة السر لا يمكن أن تتجاوز 50 حرفاً!");

    // نتحقق إن فيها حرف كبير على الأقل
    bool hasUpper = false;
    // نتحقق إن فيها حرف صغير على الأقل
    bool hasLower = false;
    // نتحقق إن فيها رقم على الأقل
    bool hasDigit = false;

    for (const QChar& c : password) {
        if (c.isUpper()) hasUpper = true;
        if (c.isLower()) hasLower = true;
        if (c.isDigit()) hasDigit = true;
    }

    if (!hasUpper)
        return ValidationResult::fail(
            "كلمة السر لازم تحتوي على حرف كبير على الأقل!");

    if (!hasLower)
        return ValidationResult::fail(
            "كلمة السر لازم تحتوي على حرف صغير على الأقل!");

    if (!hasDigit)
        return ValidationResult::fail(
            "كلمة السر لازم تحتوي على رقم على الأقل!");

    return ValidationResult::ok();
}

// ── validatePasswordConfirm ────────────────────────────────────
ValidationResult Validator::validatePasswordConfirm(
    const QString& password,
    const QString& confirm)
{
    if (confirm.isEmpty())
        return ValidationResult::fail(
            "من فضلك أكّد كلمة السر!");

    if (password != confirm)
        return ValidationResult::fail(
            "كلمة السر وتأكيد كلمة السر غير متطابقين!");

    return ValidationResult::ok();
}

// ── validateSpecies ────────────────────────────────────────────
ValidationResult Validator::validateSpecies(
    const QString& species)
{
    QString trimmed = species.trimmed();

    if (trimmed.isEmpty())
        return ValidationResult::fail(
            "نوع الحيوان لا يمكن أن يكون فارغاً!");

    if (trimmed.length() < 2)
        return ValidationResult::fail(
            "نوع الحيوان لازم يكون حرفين على الأقل!");

    if (trimmed.length() > 30)
        return ValidationResult::fail(
            "نوع الحيوان لا يمكن أن يتجاوز 30 حرفاً!");

    // حروف عربية أو إنجليزية ومسافات بس
    QRegularExpression speciesRegex(R"(^[\p{L}\s]+$)");
    if (!speciesRegex.match(trimmed).hasMatch())
        return ValidationResult::fail(
            "نوع الحيوان يجب أن يحتوي على حروف فقط!");

    return ValidationResult::ok();
}

// ── validateFood ───────────────────────────────────────────────
ValidationResult Validator::validateFood(const QString& food)
{
    QString trimmed = food.trimmed();

    if (trimmed.isEmpty())
        return ValidationResult::fail(
            "نوع الطعام لا يمكن أن يكون فارغاً!");

    if (trimmed.length() < 3)
        return ValidationResult::fail(
            "وصف الطعام لازم يكون 3 حروف على الأقل!");

    if (trimmed.length() > 100)
        return ValidationResult::fail(
            "وصف الطعام لا يمكن أن يتجاوز 100 حرف!");

    return ValidationResult::ok();
}

// ── validateAge ────────────────────────────────────────────────
ValidationResult Validator::validateAge(int age)
{
    if (age < 0)
        return ValidationResult::fail(
            "العمر لا يمكن أن يكون أقل من صفر!");

    if (age > 100)
        return ValidationResult::fail(
            "العمر لا يمكن أن يتجاوز 100 سنة!");

    return ValidationResult::ok();
}

// ── validateQuantity ───────────────────────────────────────────
ValidationResult Validator::validateQuantity(int qty)
{
    if (qty < 1)
        return ValidationResult::fail(
            "العدد لازم يكون 1 على الأقل!");

    if (qty > 20)
        return ValidationResult::fail(
            "العدد لا يمكن أن يتجاوز 20 تذكرة!");

    return ValidationResult::ok();
}

// ── applyStyle ─────────────────────────────────────────────────
void Validator::applyStyle(QLineEdit* field, bool isValid)
{
    if (isValid) {
        // أخضر فاتح لما البيانات صح
        field->setStyleSheet(
            "QLineEdit {"
            "  border: 1.5px solid #639922;"
            "  border-radius: 6px;"
            "  padding: 7px 10px;"
            "  background: #f6fbf0;"
            "}"
            );
    } else {
        // أحمر فاتح لما البيانات غلط
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