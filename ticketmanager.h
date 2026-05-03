#ifndef TICKETMANAGER_H
#define TICKETMANAGER_H

#include "ticket.h"
#include <QVector>
#include "databasemanager.h"

class MainWindow; // 1. إضافة الإعلان المُسبق للنافذة هنا

class TicketManager {
private:
    QVector<Ticket> tickets_;
    int             nextId_;
    MainWindow      *ui_window; // 2. تعريف متغير لحفظ مؤشر الواجهة
    DatabaseManager* dbManager_;
    QString currentUsername_;
public:
    void addTicketDirect(const Ticket& ticket);
    int getTodayVisitorsCount() const;
    double getTodayRevenue() const;
    QVector<Ticket> getTicketsByDate(QDate from, QDate to);

    void setCurrentUsername(const QString& username) { currentUsername_ = username; }
    QString getCurrentUsername() const { return currentUsername_; }

    // 3. تعديل المُنشئ (Constructor) ليستقبل المؤشر
  TicketManager(DatabaseManager* dbManager, MainWindow* parent = nullptr);

    Ticket          sellTicket(const QString& visitorName,
                      TicketType type,
                      int quantity);

    QVector<Ticket> getAllTickets()    const;
    int             getTotalRevenue() const;
    int             getTotalVisitors()const;
    int             getTicketsCount() const;
};
void addTicketDirect(const Ticket& ticket);

#endif // TICKETMANAGER_H