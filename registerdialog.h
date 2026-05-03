#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "usermanager.h"
#include "validator.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(UserManager* userManager,
                            QWidget*     parent = nullptr);
    ~RegisterDialog();

private slots:
    void onRegisterClicked();
    void onFullNameChanged(const QString& text);
    void onUsernameChanged(const QString& text);
    void onPasswordChanged(const QString& text);
    void onConfirmChanged(const QString& text);

private:
    Ui::RegisterDialog* ui;
    UserManager*        userManager_;

    // بتتحقق من صحة البيانات المدخلة
    // بترجع رسالة الخطأ لو في مشكلة، أو "" لو كل حاجة تمام
    QString validateInputs() const;
};

#endif // REGISTERDIALOG_H