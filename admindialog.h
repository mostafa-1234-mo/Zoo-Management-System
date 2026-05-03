#ifndef ADMINDIALOG_H
#define ADMINDIALOG_H

#include <QDialog>
#include "usermanager.h"

namespace Ui {
class AdminDialog;
}

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(UserManager* userManager,
                         QWidget*     parent = nullptr);
    ~AdminDialog();

private slots:
    void onLoginClicked();
    void onReturnPressed();

private:
    Ui::AdminDialog* ui;
    UserManager*     userManager_;
};

#endif // ADMINDIALOG_H