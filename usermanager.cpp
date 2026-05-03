#include "usermanager.h"

// ── Constructor ────────────────────────────────────────────────
UserManager::UserManager(DatabaseManager* dbManager)
    : dbManager_(dbManager)
{
    // لما البرنامج يشتغل، نجيب المستخدمين من قاعدة البيانات
    // عشان المستخدمين اللي سجلوا قبل كده يقدروا يدخلوا تاني
    if (dbManager_ && dbManager_->isConnected()) {
        loadUsersFromDatabase();
    }
}

// ── loadUsersFromDatabase ──────────────────────────────────────
void UserManager::loadUsersFromDatabase()
{
    // امسح القايمة الحالية في الذاكرة
    users_.clear();

    // جيب كل المستخدمين من قاعدة البيانات
    QVector<QMap<QString,QString>> dbUsers =
        dbManager_->loadAllUsers();

    // حوّل كل map لـ User object وضيفه في القايمة
    for (const QMap<QString,QString>& u : dbUsers) {
        // password في قاعدة البيانات محفوظ مشفّر بالفعل
        // مش محتاجين نشفّره تاني
        users_.append(User(
            u["fullName"],
            u["username"],
            u["password"],    // already hashed
            UserRole::REGULAR_USER
            ));
    }

    qDebug() << "تم تحميل" << users_.size()
             << "مستخدمين من قاعدة البيانات";
}

// ── registerUser ───────────────────────────────────────────────
bool UserManager::registerUser(const QString& fullName,
                               const QString& username,
                               const QString& password)
{
    // تحقق إن الـ username مش موجود
    if (usernameExists(username)) {
        return false;
    }

    // شفّر كلمة السر قبل الحفظ
    QString hashedPassword = User::hashPassword(password);

    // احفظ في قاعدة البيانات
    if (dbManager_ && dbManager_->isConnected()) {
        bool saved = dbManager_->saveUser(
            fullName, username, hashedPassword);

        if (!saved) {
            return false;
        }
    }

    // أضف للذاكرة كمان عشان ما نحتاجش نعمل reload
    users_.append(User(
        fullName,
        username,
        hashedPassword,
        UserRole::REGULAR_USER
        ));

    return true;
}

// ── loginUser ──────────────────────────────────────────────────
const User* UserManager::loginUser(const QString& username,
                                   const QString& password) const
{
    for (const User& user : users_) {
        if (user.getUsername() == username &&
            user.checkPassword(password)) {
            return &user;
        }
    }
    return nullptr;
}

// ── checkAdminPassword ─────────────────────────────────────────
bool UserManager::checkAdminPassword(const QString& password) const
{
    // نشفّر الـ input ونقارنه بالـ hash المحفوظ
    QString inputHash = User::hashPassword(password);
    return ADMIN_PASSWORD_HASH == inputHash;
}

// ── usernameExists ─────────────────────────────────────────────
bool UserManager::usernameExists(const QString& username) const
{
    // نفحص في الذاكرة الأول — أسرع من قاعدة البيانات
    for (const User& user : users_) {
        if (user.getUsername() == username) {
            return true;
        }
    }

    // لو مش موجود في الذاكرة، نفحص في قاعدة البيانات
    // عشان نتأكد في حالة حصل أي مشكلة في التزامن
    if (dbManager_ && dbManager_->isConnected()) {
        return dbManager_->userExists(username);
    }

    return false;
}