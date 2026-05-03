#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include "mainwindow.h"
#include "welcomedialog.h"
#include "usermanager.h"
#include "databasemanager.h"
#include "logger.h"
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    // 1. تشغيل اللوجر
    QString logsPath = QDir::currentPath() + "/logs";
    QDir().mkpath(logsPath);
    Logger::getInstance().setLogDirectory(logsPath);
    Logger::getInstance().openLogFile();

    LOG_INFO("════════════ بدأ البرنامج ════════════");

    // 2. إنشاء المانجر في الرام
    DatabaseManager* dbManager = new DatabaseManager();
    if (!dbManager->openDatabase()) {
        LOG_ERROR("فشل فتح قاعدة البيانات!");
        QMessageBox::critical(nullptr, "خطأ", "تعذر الاتصال بقاعدة البيانات!");
        return -1;
    }

    // 3. إنشاء UserManager
    UserManager* userManager = new UserManager(dbManager);

    // 4. شاشة الترحيب (Welcome Screen) ✨
    // ✅ هنا الحل النهائي: بنبعت الـ userManager وبعدين الـ dbManager زي ما الكلاس بتاعك طالب بالظبط
    WelcomeDialog welcome(userManager, dbManager);

    if (welcome.exec() != QDialog::Accepted) {
        Logger::getInstance().closeLogFile();
        return 0;
    }

    // 5. استلام الصلاحيات
    WelcomeResult result = welcome.getResult();
    UserRole role = (result == WelcomeResult::ADMIN_LOGIN) ? UserRole::ADMIN : UserRole::REGULAR_USER;
    QString username = (role == UserRole::ADMIN) ? "Mostafa" : "Employee";

    // 6. فتح MainWindow
    MainWindow window(role, username, dbManager);
    window.show();

    int exitCode = app.exec();

    LOG_INFO("البرنامج قفل بنجاح");
    Logger::getInstance().closeLogFile();

    return exitCode;
}