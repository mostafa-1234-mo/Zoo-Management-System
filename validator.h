#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>

// ── نتيجة التحقق ───────────────────────────────
// بترجعها كل دالة تحقق
struct ValidationResult {
    bool    isValid;  // true لو البيانات صح
    QString message;  // رسالة الخطأ لو مش صح

    // Constructor عشان نعمل result بسهولة
    static ValidationResult ok() {
        return {true, ""};
    }
    static ValidationResult fail(const QString& msg) {
        return {false, msg};
    }
};

class Validator
{
public:
    // ── التحقق من الاسم ────────────────────────
    // لازم يكون بين 2 و50 حرف
    // ومش يحتوي أرقام أو رموز خاصة
    static ValidationResult validateName(const QString& name);

    // ── التحقق من اسم المستخدم ─────────────────
    // لازم يكون بين 4 و20 حرف
    // حروف إنجليزية وأرقام و underscore بس
    static ValidationResult validateUsername(
        const QString& username);

    // ── التحقق من كلمة السر ────────────────────
    // لازم تكون 6 حروف على الأقل
    // وتحتوي حرف كبير وحرف صغير ورقم على الأقل
    static ValidationResult validatePassword(
        const QString& password);

    // ── التحقق إن كلمتا السر متطابقتين ──────────
    static ValidationResult validatePasswordConfirm(
        const QString& password,
        const QString& confirm);

    // ── التحقق من اسم النوع (species) ──────────
    // حروف عربية أو إنجليزية بس
    static ValidationResult validateSpecies(
        const QString& species);

    // ── التحقق من الطعام ───────────────────────
    // مش فاضي وأقل من 100 حرف
    static ValidationResult validateFood(const QString& food);

    // ── التحقق من العمر ────────────────────────
    // بين 0 و100 سنة
    static ValidationResult validateAge(int age);

    // ── التحقق من العدد ────────────────────────
    // بين 1 و20
    static ValidationResult validateQuantity(int qty);

    // ── دالة مساعدة بتلوّن الـ QLineEdit ────────
    // أخضر لو صح، أحمر لو غلط
    static void applyStyle(QLineEdit* field,
                           bool isValid);

    // ── دالة بتعرض رسالة الخطأ في QLabel ────────
    static void showError(QLabel*        label,
                          const QString& message);

    // ── دالة بتخبي رسالة الخطأ ──────────────────
    static void hideError(QLabel* label);
};

#endif // VALIDATOR_H