#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QVector>
#include <QCryptographicHash>
#include "databasemanager.h"

enum class UserRole {
    REGULAR_USER,
    ADMIN
};

class User {
public:
    User(const QString& fullName,
         const QString& username,
         const QString& hashedPassword,
         UserRole role)
        : fullName_(fullName),
        username_(username),
        hashedPassword_(hashedPassword),
        role_(role)
    {}

    QString  getFullName() const { return fullName_; }
    QString  getUsername() const { return username_; }
    UserRole getRole()     const { return role_; }

    bool checkPassword(const QString& inputPassword) const {
        // نشفّر الـ input ونقارنه بالمشفّر المحفوظ
        QString hashed = hashPassword(inputPassword);
        return hashedPassword_ == hashed;
    }

    // دالة static لتشفير كلمة السر
    // static عشان نقدر نناديها من غير ما نعمل object
    static QString hashPassword(const QString& password) {
        QByteArray data = password.toUtf8();
        QByteArray hashed = QCryptographicHash::hash(
            data,
            QCryptographicHash::Sha256
            );
        return QString(hashed.toHex());
    }

private:
    QString  fullName_;
    QString  username_;        // ✅ التعديل هنا: طلعنا دي فوق
    QString  hashedPassword_;  // ✅ التعديل هنا: نزلنا دي تحتها عشان الترتيب يظبط
    UserRole role_;
};

class UserManager {
public:
    // ── التغيير الأساسي ────────────────────────
    // دلوقتي UserManager بياخد DatabaseManager*
    // عشان يحفظ ويجيب المستخدمين من قاعدة البيانات
    explicit UserManager(DatabaseManager* dbManager);

    // تسجيل مستخدم جديد
    bool registerUser(const QString& fullName,
                      const QString& username,
                      const QString& password);

    // تسجيل دخول
    const User* loginUser(const QString& username,
                          const QString& password) const;

    // التحقق من كلمة سر المدير
    bool checkAdminPassword(const QString& password) const;

    // التحقق إن الـ username موجود
    bool usernameExists(const QString& username) const;

    // تحميل المستخدمين من قاعدة البيانات عند بدء البرنامج
    void loadUsersFromDatabase();

private:
    QVector<User>    users_;
    DatabaseManager* dbManager_;

    // كلمة سر المدير مشفّرة
    // القيمة دي هي SHA-256 لـ "ZooAdmin2026"
    const QString ADMIN_PASSWORD_HASH =
        User::hashPassword("ZooAdmin2026");
};

#endif // USERMANAGER_H