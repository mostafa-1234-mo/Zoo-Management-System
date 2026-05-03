#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include "animalmanager.h"
#include "ticketmanager.h"


namespace Ui {
class DashboardWidget;
}

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(AnimalManager*  animalManager,
                             TicketManager*  ticketManager,
                             QWidget*        parent = nullptr);
    ~DashboardWidget();

    void refreshDashboard();

private:
    Ui::DashboardWidget* ui;
    AnimalManager*       animalManager_;
    TicketManager*       ticketManager_;

    // ── الدوال دي كانت ناقصة ──────────────────
    void updateStatCards();
    void updateFeedingProgress();
    void updateRecentTickets();
};

#endif // DASHBOARDWIDGET_H