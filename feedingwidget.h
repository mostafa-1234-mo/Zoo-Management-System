#ifndef FEEDINGWIDGET_H
#define FEEDINGWIDGET_H

#include <QWidget>
#include <QTimer>
#include "animalmanager.h"
#include "databasemanager.h"

namespace Ui {
class FeedingWidget;
}

class FeedingWidget : public QWidget
{
    Q_OBJECT

public:
    void setAdminMode(bool isAdmin);
    explicit FeedingWidget(AnimalManager*   animalManager,
                           DatabaseManager* dbManager,
                           QWidget*         parent = nullptr);
    ~FeedingWidget();

    // بتحدث الجدول كله
    void refreshTable();

private slots:
    // بيتنادى لما المستخدم يضغط زرار "تسجيل التغذية" في أي صف
    void onFeedButtonClicked(int animalId);

    // بيتنادى لما المستخدم يضغط "إعادة تعيين"
    void onResetClicked();

    // بيتنادى كل دقيقة عشان يفحص لو في حيوانات لم تتغذَّ
    void onTimerTick();

private:
    bool isAdminMode_ = false;
    Ui::FeedingWidget* ui;
    AnimalManager*     animalManager_;
    DatabaseManager*   dbManager_;
    QTimer*            checkTimer_; // بيشتغل كل دقيقة

    // دوال مساعدة
    void updateSummary();  // بتحدث النص والـ ProgressBar
    void checkAlerts();    // بتفحص لو في حيوانات محتاجة تغذية
    void addRowToTable(const Animal& animal, int row); // بتضيف صف في الجدول
};

#endif // FEEDINGWIDGET_H