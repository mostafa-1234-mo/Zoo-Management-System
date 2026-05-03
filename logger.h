#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QMutex>

// ── مستويات الـ Log ────────────────────────────
enum class LogLevel {
    INFO,    // معلومة عادية — دخول مستخدم، إضافة حيوان
    WARNING, // تحذير — محاولة دخول فاشلة
    ERROR_L  // خطأ — مشكلة في قاعدة البيانات
    // سميناه ERROR_L عشان ERROR اسم محجوز في Windows
};

class Logger
{
public:
    // ── Singleton Pattern ──────────────────────
    // Singleton معناها إن في instance واحدة بس من الكلاس
    // كل الكود في البرنامج بيستخدم نفس الـ Logger
    // getInstance() بترجع الـ instance الوحيدة دي
    static Logger& getInstance();

    // ── دوال التسجيل ───────────────────────────
    void log(LogLevel          level,
             const QString&    message,
             const QString&    source = "");

    // Shortcuts عشان الكتابة تبقى أسهل
    void info (const QString& message,
              const QString& source = "");
    void warn (const QString& message,
              const QString& source = "");
    void error(const QString& message,
               const QString& source = "");

    // ── إعدادات ────────────────────────────────
    // بتحدد مجلد الـ logs
    void setLogDirectory(const QString& dir);

    // بتفتح ملف اليوم
    bool openLogFile();

    // بتقفل الملف عند إغلاق البرنامج
    void closeLogFile();

    // ── منع النسخ في Singleton ─────────────────
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

private:
    // Constructor خاص عشان Singleton
    Logger();
    ~Logger();

    // ── البيانات الداخلية ───────────────────────
    QFile*       logFile_;    // ملف الـ log
    QTextStream* logStream_;  // stream للكتابة فيه
    QString      logDir_;     // مجلد الـ logs
    QMutex       mutex_;      // عشان thread safety

    // بتحول الـ LogLevel لنص
    QString levelToString(LogLevel level) const;

    // بترجع اسم ملف اليوم
    QString getTodayFileName() const;
};

// ── Macros عشان الكتابة تبقى أسهل ──────────────
// بدل ما تكتب Logger::getInstance().info(...)
// بتكتب LOG_INFO(...)
#define LOG_INFO(msg)  Logger::getInstance().info(msg,  __FUNCTION__)
#define LOG_WARN(msg)  Logger::getInstance().warn(msg,  __FUNCTION__)
#define LOG_ERROR(msg) Logger::getInstance().error(msg, __FUNCTION__)

#endif // LOGGER_H