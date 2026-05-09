#include "databasemanager.h"
#include "logger.h"
#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QSqlQuery>

// ── Constructor ────────────────────────────────────────────────
DatabaseManager::DatabaseManager()
{
    openDatabase();
}

// ── Destructor ─────────────────────────────────────────────────
DatabaseManager::~DatabaseManager()
{
    if (db_.isOpen()) {
        db_.close();
    }
}

// ── openDatabase ───────────────────────────────────────────────
bool DatabaseManager::openDatabase()
{
    // 1. فحص الاتصال: لو مفتوح استخدمه، لو لأ افتح واحد جديد
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db_ = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db_ = QSqlDatabase::addDatabase("QSQLITE");
    }

    // 2. تحديد مسار القاعدة
    QString dbPath = QCoreApplication::applicationDirPath() + "/zoo_park_v3.sqlite";
    db_.setDatabaseName(dbPath);

    // 3. محاولة الفتح
    if (!db_.open()) {
        LOG_ERROR("Failed to open database: " + db_.lastError().text());
        return false;
    }

    LOG_INFO("Database opened successfully: " + dbPath);
    return createTables();
}

bool DatabaseManager::isConnected() const { return db_.isOpen(); }

// ── createTables ───────────────────────────────────────────────
bool DatabaseManager::createTables()
{
    QSqlQuery query(db_);

    // جدول الحيوانات
    query.exec("CREATE TABLE IF NOT EXISTS animals ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, species TEXT, age INTEGER, "
               "health TEXT, food TEXT, is_fed INTEGER DEFAULT 0, "
               "last_fed TEXT, image_path TEXT)");

    // 🚨 السطر السحري: ده هيمسح الجدول القديم بكل مشاكله
    //query.exec("DROP TABLE IF EXISTS tickets");

    // جدول التذاكر (هيتعمل من جديد بالعمود السحري user_name)
    query.exec("CREATE TABLE IF NOT EXISTS tickets ("
               "id INTEGER PRIMARY KEY, visitor_name TEXT, "
               "type TEXT, quantity INTEGER, total_price INTEGER, "
               "sale_time TEXT, user_name TEXT)");

    // جدول المستخدمين
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "fullname TEXT, username TEXT UNIQUE, password TEXT)");

    return true;
}

// ── الدوال الخاصة بالحيوانات (بدون تغيير) ───────────────────────────
bool DatabaseManager::saveAnimal(const Animal& animal)
{
    QSqlQuery query(db_);
    query.prepare("REPLACE INTO animals (id, name, species, age, health, food, is_fed, last_fed, image_path) "
                  "VALUES (:id, :name, :species, :age, :health, :food, :is_fed, :last_fed, :image_path)");

    query.bindValue(":id", animal.getId());
    query.bindValue(":name", animal.getName());
    query.bindValue(":species", animal.getSpecies());
    query.bindValue(":age", animal.getAge());
    query.bindValue(":health", animal.healthToString());
    query.bindValue(":food", animal.getFood());
    query.bindValue(":is_fed", animal.getFed() ? 1 : 0);
    query.bindValue(":last_fed", animal.getLastFedTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":image_path", animal.getImagePath());

    return query.exec();
}

bool DatabaseManager::deleteAnimal(int id)
{
    QSqlQuery query(db_);
    query.prepare("DELETE FROM animals WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool DatabaseManager::updateFeedingStatus(int animalId, bool fed)
{
    QSqlQuery query(db_);
    query.prepare("UPDATE animals SET is_fed = :fed, last_fed = :time WHERE id = :id");
    query.bindValue(":id", animalId);
    query.bindValue(":fed", fed ? 1 : 0);
    query.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    return query.exec();
}

QVector<Animal> DatabaseManager::loadAllAnimals()
{
    QVector<Animal> animals;
    QSqlQuery query("SELECT * FROM animals", db_);

    while (query.next()) {
        Animal a(query.value("id").toInt(),
                 query.value("name").toString(),
                 query.value("species").toString(),
                 query.value("age").toInt(),
                 HealthStatus::OK,
                 query.value("food").toString());

        a.setImagePath(query.value("image_path").toString());
        a.setFed(query.value("is_fed").toInt() == 1);

        QString timeStr = query.value("last_fed").toString();
        if (!timeStr.isEmpty()) {
            a.setLastFedTime(QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss"));
        }
        animals.append(a);
    }
    return animals;
}

// ── الدوال الخاصة بالتذاكر (تم تعديلها للفلترة حسب المستخدم) ─────────────

bool DatabaseManager::saveTicket(const Ticket& ticket, const QString& currentUsername)
{
    QSqlQuery query(db_);
    query.prepare("REPLACE INTO tickets (id, visitor_name, type, quantity, total_price, sale_time, user_name) "
                  "VALUES (:id, :name, :type, :qty, :price, :time, :user)");

    query.bindValue(":id", ticket.getId());
    query.bindValue(":name", ticket.getVisitorName());
    query.bindValue(":type", ticket.typeToString());
    query.bindValue(":qty", ticket.getQuantity());
    query.bindValue(":price", ticket.getTotalPrice());
    query.bindValue(":time", ticket.getSaleTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":user", currentUsername); // 👈 ربط التذكرة بالموظف الحالي

    if (!query.exec()) {
        // لو فشل، هيطبعلك السبب بالظبط في شاشة الـ Output
        qDebug() << "❌ Failed to save ticket to database:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ Ticket saved to database successfully for user:" << currentUsername;
    return true;
}

QVector<Ticket> DatabaseManager::loadAllTickets(const QString& currentUsername, bool isAdmin) {
    QVector<Ticket> tickets;
    QSqlQuery query(db_);

    query.prepare("SELECT * FROM tickets ORDER BY sale_time DESC");

    if (!query.exec()) {
        qDebug() << "❌ Failed to fetch tickets:" << query.lastError().text();
        return tickets;
    }

    while (query.next()) {
        Ticket t;
        t.setId(query.value("id").toInt());
        t.setVisitorName(query.value("visitor_name").toString());
        t.setQuantity(query.value("quantity").toInt());
        t.setTotalPrice(query.value("total_price").toDouble());
        t.setSaleTime(QDateTime::fromString(query.value("sale_time").toString(), "yyyy-MM-dd HH:mm:ss"));

        // 🌟 التصليح هنا: قراءة النوع كنص وتحويله لـ Enum صح 🌟
        QString typeStr = query.value("type").toString();
        TicketType type;
        if (typeStr == "Child" || typeStr == "أطفال") type = TicketType::CHILD;
        else if (typeStr == "Family" || typeStr == "عائلة") type = TicketType::FAMILY;
        else if (typeStr == "Student" || typeStr == "طالب") type = TicketType::STUDENT;
        else type = TicketType::ADULT;

        t.setType(type);

        tickets.append(t);
    }
    return tickets;
}
QVector<Ticket> DatabaseManager::getTicketsByDate(QDate from, QDate to, const QString& currentUsername)
{
    QVector<Ticket> tickets;
    QSqlQuery query(db_);

    // 👈 شيلنا شرط الـ user_name واكتفينا بالتاريخ بس
    query.prepare("SELECT * FROM tickets WHERE date(sale_time) BETWEEN :from AND :to");

    query.bindValue(":from", from.toString("yyyy-MM-dd"));
    query.bindValue(":to", to.toString("yyyy-MM-dd"));

    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            QString name = query.value("visitor_name").toString();
            QString typeStr = query.value("type").toString();
            int qty = query.value("quantity").toInt();
            QDateTime saleTime = QDateTime::fromString(query.value("sale_time").toString(), "yyyy-MM-dd HH:mm:ss");

            TicketType type;
            if (typeStr == "Child") type = TicketType::CHILD;
            else if (typeStr == "Family") type = TicketType::FAMILY;
            else if (typeStr == "Student") type = TicketType::STUDENT;
            else type = TicketType::ADULT;

            tickets.append(Ticket(id, name, type, qty, saleTime));
        }
    }
    return tickets;
}
// ── الدوال الخاصة بالمستخدمين ──────────────────────────────────
bool DatabaseManager::saveUser(const QString& full, const QString& user, const QString& pass)
{
    QSqlQuery query(db_);
    query.prepare("INSERT INTO users (fullname, username, password) VALUES (?, ?, ?)");
    query.addBindValue(full); query.addBindValue(user); query.addBindValue(pass);
    return query.exec();
}

bool DatabaseManager::userExists(const QString& username)
{
    QSqlQuery query(db_);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(username);
    return query.exec() && query.next() && query.value(0).toInt() > 0;
}

QVector<QMap<QString,QString>> DatabaseManager::loadAllUsers()
{
    QVector<QMap<QString,QString>> users;
    QSqlQuery query("SELECT * FROM users", db_);
    while (query.next()) {
        QMap<QString,QString> u;
        u["username"] = query.value("username").toString();
        u["password"] = query.value("password").toString();
        users.append(u);
    }
    return users;
}