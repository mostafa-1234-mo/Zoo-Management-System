#ifndef TICKET_H
#define TICKET_H

#include <QString>
#include <QDateTime>

enum class TicketType {
    ADULT,
    CHILD,
    FAMILY,
    STUDENT
};

class Ticket {
private:
    int         id_;
    QString     visitorName_;
    TicketType  type_;
    int         quantity_;
    int         totalPrice_;
    QDateTime   saleTime_;

public:
    Ticket();
    Ticket(int id,
           const QString& visitorName,
           TicketType type,
           int quantity,
           QDateTime saleTime = QDateTime::currentDateTime());

    // ── Getters ──────────────────────────────────────────
    int         getId()          const;
    QString     getVisitorName() const;
    TicketType  getType()        const;
    int         getQuantity()    const;
    double getTotalPrice() const;
    QDateTime   getSaleTime()    const;

    // ── Setters (الـدوال الـلي ضـفنـاها) ──────────────────
    void setId(int id);
    void setVisitorName(const QString& name);
    void setType(TicketType type);
    void setQuantity(int quantity);
   void setTotalPrice(double price);
    void setSaleTime(const QDateTime& time);

    // ── Helpers ──────────────────────────────────────────
    QString typeToString()         const;
    QString getSaleTimeFormatted() const;

    // ── Static functions ─────────────────────────────────
    static int      getPrice(TicketType type);
    static QString typeToString(TicketType type);
};

#endif // TICKET_H