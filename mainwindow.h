#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTimer>
#include <QLabel>
#include "animalmanager.h"
#include "ticketmanager.h"
#include "databasemanager.h"
#include "animalwidget.h"
#include "ticketwidget.h"
#include "feedingwidget.h"
#include "dashboardwidget.h"
#include "reportwidget.h"
#include "usermanager.h"
#include "logger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // ── التغيير هنا — بياخد DatabaseManager* كمان ──
 explicit MainWindow(UserRole role, const QString& username, DatabaseManager* dbManager, QWidget *parent = nullptr);

     ~MainWindow();


private slots:
    void onTabChanged(int index);
    void onAutoRefresh();

private:
    void setupTabsByRole();
     Ui::MainWindow*  ui;
    UserRole currentRole_;
    QString currentUserName_;


    // الـ Managers
    AnimalManager*   animalManager_;
    TicketManager*   ticketManager_;
    DatabaseManager* dbManager_;

    // الـ Widgets
    DashboardWidget* dashboardWidget_;
    AnimalWidget*    animalWidget_;
    TicketWidget*    ticketWidget_;
    FeedingWidget*   feedingWidget_;
    ReportWidget*    reportWidget_;

    // الـ TabWidget
    QTabWidget*      tabWidget_;

    // Timer + StatusBar
    QTimer*          refreshTimer_;
    QLabel*          statusLabel_;

    // الدوال
    void setupStatusBar();
    void applyRolePermissions();

    // ── دوال جديدة للبيانات ────────────────────
    void loadDataFromDatabase();
    void saveDataToDatabase();
};
// ── loadDataFromDatabase ───────────────────────────────────────


#endif // MAINWINDOW_H