#include "logger.h"
#include <QDebug>

// ── Constructor ────────────────────────────────────────────────
Logger::Logger()
    : logFile_(nullptr),
    logStream_(nullptr),
    logDir_("logs")  // المجلد الافتراضي اسمه logs
{
    // افتح ملف اليوم تلقائياً لما الـ Logger يتعمل
    openLogFile();
}

// ── Destructor ─────────────────────────────────────────────────
Logger::~Logger()
{
    closeLogFile();
}

// ── getInstance ────────────────────────────────────────────────
Logger& Logger::getInstance()
{
    // static local variable — بتتعمل مرة واحدة بس
    // وبتفضل موجودة طول عمر البرنامج
    static Logger instance;
    return instance;
}

// ── setLogDirectory ────────────────────────────────────────────
void Logger::setLogDirectory(const QString& dir)
{
    logDir_ = dir;
}

// ── getTodayFileName ───────────────────────────────────────────
QString Logger::getTodayFileName() const
{
    // اسم الملف: zoo_log_2024-01-15.txt
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    return logDir_ + "/zoo_log_" + date + ".txt";
}

// ── openLogFile ────────────────────────────────────────────────
bool Logger::openLogFile()
{
    // اعمل مجلد logs لو مش موجود
    QDir dir;
    if (!dir.exists(logDir_)) {
        if (!dir.mkpath(logDir_)) {
            qDebug() << "فشل إنشاء مجلد logs!";
            return false;
        }
    }

    // لو فيه ملف مفتوح من قبل، اقفله
    closeLogFile();

    // افتح ملف اليوم
    // Append معناها نضيف في آخر الملف مش نمسحه
    logFile_ = new QFile(getTodayFileName());
    if (!logFile_->open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "فشل فتح ملف log:"
                 << logFile_->fileName();
        delete logFile_;
        logFile_ = nullptr;
        return false;
    }

    logStream_ = new QTextStream(logFile_);

    // عشان يكتب العربي صح
    logStream_->setEncoding(QStringConverter::Utf8);

    return true;
}

// ── closeLogFile ───────────────────────────────────────────────
void Logger::closeLogFile()
{
    if (logStream_) {
        logStream_->flush(); // تأكد إن كل حاجة اتكتبت
        delete logStream_;
        logStream_ = nullptr;
    }
    if (logFile_) {
        logFile_->close();
        delete logFile_;
        logFile_ = nullptr;
    }
}

// ── levelToString ──────────────────────────────────────────────
QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::INFO:    return "INFO ";
    case LogLevel::WARNING: return "WARN ";
    case LogLevel::ERROR_L: return "ERROR";
    }
    return "INFO ";
}

// ── log ────────────────────────────────────────────────────────
void Logger::log(LogLevel       level,
                 const QString& message,
                 const QString& source)
{
    // QMutexLocker بيقفل الـ mutex تلقائياً
    // ويفتحه لما يخرج من الـ scope
    // عشان لو في threads متعددة ما يحصلش تعارض
    QMutexLocker locker(&mutex_);

    // ── تجهيز السطر ────────────────────────────
    QString timestamp = QDateTime::currentDateTime()
                            .toString("yyyy-MM-dd hh:mm:ss");

    QString levelStr = levelToString(level);

    QString line;
    if (source.isEmpty()) {
        line = QString("[%1] [%2] %3")
        .arg(timestamp)
            .arg(levelStr)
            .arg(message);
    } else {
        line = QString("[%1] [%2] [%3] %4")
        .arg(timestamp)
            .arg(levelStr)
            .arg(source)
            .arg(message);
    }

    // ── الكتابة في الملف ───────────────────────
    // تحقق إن ملف اليوم هو اللي مفتوح
    // لو اتغير اليوم، افتح ملف جديد
    if (logFile_ &&
        logFile_->fileName() != getTodayFileName()) {
        openLogFile(); // يوم جديد — افتح ملف جديد
    }

    if (logStream_) {
        *logStream_ << line << "\n";
        logStream_->flush(); // اكتب فوراً في الملف
    }

    // ── الطباعة في الـ Qt Debug Output كمان ─────
    qDebug().noquote() << line;
}

// ── Shortcut Functions ─────────────────────────────────────────
void Logger::info(const QString& message, const QString& source)
{
    log(LogLevel::INFO, message, source);
}

void Logger::warn(const QString& message, const QString& source)
{
    log(LogLevel::WARNING, message, source);
}

void Logger::error(const QString& message, const QString& source)
{
    log(LogLevel::ERROR_L, message, source);
}