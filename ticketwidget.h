#ifndef TICKETWIDGET_H
#define TICKETWIDGET_H

#include <QWidget>
#include "ticketmanager.h"
#include "databasemanager.h"

namespace Ui {
class TicketWidget;
}

class TicketWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TicketWidget(TicketManager* manager, DatabaseManager* dbManager, QWidget* parent = nullptr);
    ~TicketWidget();

    // ✅ تم تعديل الاسم هنا لـ refreshTable عشان يطابق باقي المشروع
    void refreshTable();

private slots:
    // بيتنادى لما المستخدم يضغط "بيع التذكرة"
    void onSellClicked();

    // السلوت الجديد والوحيد للطباعة
    void onPrintClicked();

    // تحديث الإجمالي تلقائياً عند تغيير النوع أو العدد
    void onTicketDetailsChanged();



private:
    Ui::TicketWidget* ui;
    TicketManager* ticketManager_;
    DatabaseManager* dbManager_;
    // ❌ تم حذف TicketManager* manager_; الزيادة عشان نمنع التكرار

    void addRowToTable(const Ticket& ticket, int row); // بتضيف صف في الجدول

    // بترجع السعر حسب الـ index في الـ ComboBox
    int getPriceByIndex(int index) const;

    // بتحول الـ index لـ TicketType
    TicketType getTypeByIndex(int index) const;
};

#endif // TICKETWIDGET_H