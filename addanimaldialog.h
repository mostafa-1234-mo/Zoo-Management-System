#ifndef ADDANIMALDIALOG_H
#define ADDANIMALDIALOG_H

#include <QDialog>
#include <QLabel>    // تم إضافة مكتبة QLabel
#include "animal.h"  // محتاجينه عشان enum HealthStatus
#include "validator.h"

namespace Ui {
class AddAnimalDialog;
}

class AddAnimalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddAnimalDialog(QWidget* parent = nullptr);
    ~AddAnimalDialog();

    // دوال بترجع البيانات اللي المستخدم دخلها
    // بنناديها من AnimalWidget بعد ما المستخدم يضغط OK
    QString      getAnimalName()    const;
    QString      getAnimalSpecies() const;
    int          getAnimalAge()     const;
    HealthStatus getAnimalHealth()  const;
    QString      getAnimalFood()    const;
    QString getImagePath() const { return currentImagePath; }

private slots:
    // تم إضافة الـ slots الخاصة بالـ Validation
    void onNameChanged(const QString& text);
    void onSpeciesChanged(const QString& text);
    void onFoodChanged(const QString& text);



    void on_btnChooseImage_clicked();

private:
    Ui::AddAnimalDialog* ui;
    QString currentImagePath;

    // تم إضافة QLabel لعرض أخطاء الإدخال

};

#endif // ADDANIMALDIALOG_H