#include "ticketmanager.h"
#include "mainwindow.h"
#include "databasemanager.h"
#include "logger.h"
#include <QSqlQuery>
#include <QVariant>

// ── Constructor ────────────────────────────────────────────────
// التعديل: نستقبل مؤشر الداتابيز ومؤشر النافذة ونربطهم
TicketManager::TicketManager(DatabaseManager* dbManager, MainWindow* parent)
    : nextId_(1)
    , ui_window(parent)
    , dbManager_(dbManager)
{
}

Ticket TicketManager::sellTicket(const QString& visitorName,
                                 TicketType type,
                                 int quantity)
{
    Ticket t(nextId_++, visitorName, type, quantity);
    tickets_.append(t);

    // ── حفظ في الداتابيز ──
    if (dbManager_) {
       dbManager_->saveTicket(t, currentUsername_);
    }

    // ── سجل عملية البيع ──
    LOG_INFO("Ticket sold — Visitor: " + visitorName +
             " | Type: " + t.typeToString() +
             " | Quantity: " + QString::number(quantity) +
             " | Total: " +
             QString::number(t.getTotalPrice()) + " EGP");
    return t;
}

QVector<Ticket> TicketManager::getAllTickets() const {
    // 👈 اقرأ من الذاكرة (جيبك) اللي إنت لسه ضايف فيها التذكرة
    return tickets_;
}
   int TicketManager::getTicketsCount()  const { return tickets_.size(); }

int TicketManager::getTotalRevenue() const {
    int sum = 0;
    for (const Ticket& t : tickets_) sum += t.getTotalPrice();
    return sum;
}

int TicketManager::getTotalVisitors() const {
    int sum = 0;
    for (const Ticket& t : tickets_) sum += t.getQuantity();
    return sum;
}

// ── تصفية التذاكر بالتاريخ ──
QVector<Ticket> TicketManager::getTicketsByDate(QDate from, QDate to) {
    if (dbManager_) {
        // بيفوض المهمة للداتابيز مانجر
        return dbManager_->getTicketsByDate(from, to, currentUsername_);
    }
    return QVector<Ticket>();
}

int TicketManager::getTodayVisitorsCount() const
{
    QSqlQuery query;
    query.prepare("SELECT SUM(quantity) FROM tickets WHERE date(sale_time) = date('now', 'localtime')");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

double TicketManager::getTodayRevenue() const
{
    QSqlQuery query;
    query.prepare("SELECT SUM(total_price) FROM tickets WHERE date(sale_time) = date('now', 'localtime')");
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

void TicketManager::addTicketDirect(const Ticket& ticket)
{
    tickets_.append(ticket);
    if (ticket.getId() >= nextId_) {
        nextId_ = ticket.getId() + 1;
    }
}