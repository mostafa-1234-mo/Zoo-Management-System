#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVector>
#include <QString>
#include <QDate>
#include <QMap>
#include <QDebug>
#include "animal.h"
#include "ticket.h"

class DatabaseManager
{
public:
    // Constructor — بيفتح قاعدة البيانات وبيعمل الجداول لو مش موجودة
    DatabaseManager();

    // Destructor — بيقفل قاعدة البيانات
    ~DatabaseManager();

    bool openDatabase();   // بتفتح الـ connection
    bool isConnected() const;

    // ── Animal Operations ──────────────────────────
    bool            saveAnimal(const Animal& animal);
    bool            updateAnimal(const Animal& animal);
    bool            deleteAnimal(int id);
    QVector<Animal> loadAllAnimals();
    bool            updateFeedingStatus(int animalId, bool fed);

    // ── Ticket Operations (تم التعديل لإضافة اسم المستخدم) ──

    // حفظ تذكرة مع ربطها بالموظف الحالي
    bool            saveTicket(const Ticket& ticket, const QString& currentUsername);

    // ضيف bool isAdmin = false
    QVector<Ticket> loadAllTickets(const QString& currentUsername, bool isAdmin = false);

    // تصفية التذاكر بالتاريخ والموظف
    QVector<Ticket> getTicketsByDate(QDate from, QDate to, const QString& currentUsername);

    // ── User Operations ────────────────────────────
    bool saveUser(const QString& fullName,
                  const QString& username,
                  const QString& hashedPassword);

    QVector<QMap<QString,QString>> loadAllUsers();
    bool userExists(const QString& username);

private:
    QSqlDatabase db_; // الـ connection بتاع قاعدة البيانات

    // دوال مساعدة خاصة
    bool createTables();   // بتعمل الجداول لو مش موجودة
};

#endif // DATABASEMANAGER_H