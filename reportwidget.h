#ifndef REPORTWIDGET_H
#define REPORTWIDGET_H

#include <QWidget>
#include "ticket.h"
#include <QVector>

// 1. تعريف مسبق
class TicketManager;

namespace Ui {
class ReportWidget;
}

class ReportWidget : public QWidget
{
    Q_OBJECT

public:
    // 2. تعديل الـ Constructor عشان يستقبل الـ Manager
    explicit ReportWidget(TicketManager* ticketManager, QWidget *parent = nullptr);
    ~ReportWidget();

    // 3. إضافة دالة التحديث
    void onRefreshClicked();

private slots:
    void on_btnFilter_clicked();


private:
    Ui::ReportWidget *ui;

    // 4. متغير عشان نحفظ الـ Manager جواه
    TicketManager* ticketManager_;
    QString buildTableHtml(const QVector<Ticket>& tickets);
};

#endif // REPORTWIDGET_H