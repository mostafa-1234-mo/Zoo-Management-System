#ifndef ANIMALWIDGET_H
#define ANIMALWIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include "animalmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "databasemanager.h"

// ده forward declaration عشان نقول للـ compiler
// إن في class اسمها Ui::AnimalWidget موجودة
// من غير ما نعمل include كامل
namespace Ui {
class AnimalWidget;
}

class AnimalWidget : public QWidget
{
    // Q_OBJECT ده macro ضروري جداً
    // من غير ما تكتبه، الـ signals والـ slots مش هتشتغل
    Q_OBJECT

public:
    void setAdminMode(bool isAdmin);
    // Constructor بياخد pointer للـ AnimalManager
    // عشان الـ widget تشتغل على نفس البيانات مش نسخة منها
  explicit AnimalWidget(AnimalManager* manager, DatabaseManager* dbManager, QWidget* parent = nullptr);

    // Destructor بيمسح الـ UI من الذاكرة
    ~AnimalWidget();

    // بتحدث الجدول كله من البيانات الموجودة في الـ manager
    void refreshTable();

private slots:
    // ده slot بيتنادى لما المستخدم يضغط "إضافة حيوان"
    void onAddClicked();

    // ده slot بيتنادى لما المستخدم يضغط "حذف"
    void onDeleteClicked();

    // ده slot بيتنادى كل ما المستخدم يكتب حرف في خانة البحث
    void onSearchTextChanged(const QString& text);



private:

    Ui::AnimalWidget* ui;       // pointer للـ UI اللي اتصمم في الـ Designer
    AnimalManager*    manager_; // pointer للـ manager — مش بنمسحه احنا
    DatabaseManager* dbManager_;
    // دالة مساعدة بتضيف صف واحد في الجدول
    void addRowToTable(const Animal& animal, int row);

    // دالة بتلون الخلية حسب الحالة الصحية
    void colorHealthCell(QTableWidgetItem* item, HealthStatus health);
};

#endif // ANIMALWIDGET_H