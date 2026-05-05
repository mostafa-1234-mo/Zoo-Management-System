#include "addanimaldialog.h"
#include "ui_addanimaldialog.h"
#include <QFileDialog>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

// ── Constructor ────────────────────────────────────────────────
AddAnimalDialog::AddAnimalDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::AddAnimalDialog)
{
    ui->setupUi(this);


    // عنوان الـ dialog
    setWindowTitle("Add New Animal");

    // حجم ثابت للـ dialog — مش هيتكبر أو يتصغر
    setFixedSize(400, 280);
    // تنسيق رسالة الخطأ
    ui->labelError->setStyleSheet(
        "color: #a32d2d; font-size: 12px;"
        );
    ui->labelError->hide();

    // ربط التحقق الفوري
    connect(ui->lineEditName,
            &QLineEdit::textChanged,
            this,
            &AddAnimalDialog::onNameChanged);

    connect(ui->lineEditSpecies,
            &QLineEdit::textChanged,
            this,
            &AddAnimalDialog::onSpeciesChanged);

    connect(ui->lineEditFood,
            &QLineEdit::textChanged,
            this,
            &AddAnimalDialog::onFoodChanged);
}

// ── Destructor ─────────────────────────────────────────────────
AddAnimalDialog::~AddAnimalDialog()
{
    delete ui;
}

// ── Getters ────────────────────────────────────────────────────

QString AddAnimalDialog::getAnimalName() const
{
    // trimmed() بتشيل المسافات الزيادة من الأول والآخر
    return ui->lineEditName->text().trimmed();
}

QString AddAnimalDialog::getAnimalSpecies() const
{
    return ui->lineEditSpecies->text().trimmed();
}

int AddAnimalDialog::getAnimalAge() const
{
    // value() بترجع الرقم من الـ QSpinBox
    return ui->spinBoxAge->value();
}

HealthStatus AddAnimalDialog::getAnimalHealth() const
{
    // currentIndex() بترجع رقم العنصر المختار في الـ QComboBox
    // 0 = جيدة، 1 = تحتاج متابعة، 2 = حرجة
    switch (ui->comboHealth->currentIndex()) {
    case 0: return HealthStatus::OK;
    case 1: return HealthStatus::WARNING;
    case 2: return HealthStatus::CRITICAL;
    default: return HealthStatus::OK;
    }
}

QString AddAnimalDialog::getAnimalFood() const
{
    return ui->lineEditFood->text().trimmed();
}
// ── onNameChanged ──────────────────────────────────────────────
void AddAnimalDialog::onNameChanged(const QString& text)
{
    auto result = Validator::validateName(text);
    Validator::applyStyle(ui->lineEditName,
                          result.isValid || text.isEmpty());
    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelError, result.message);
    else
        Validator::hideError(ui->labelError);
}

// ── onSpeciesChanged ───────────────────────────────────────────
void AddAnimalDialog::onSpeciesChanged(const QString& text)
{
    auto result = Validator::validateSpecies(text);
    Validator::applyStyle(ui->lineEditSpecies,
                          result.isValid || text.isEmpty());
    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelError, result.message);
    else
        Validator::hideError(ui->labelError);
}

// ── onFoodChanged ──────────────────────────────────────────────
void AddAnimalDialog::onFoodChanged(const QString& text)
{
    auto result = Validator::validateFood(text);
    Validator::applyStyle(ui->lineEditFood,
                          result.isValid || text.isEmpty());
    if (!result.isValid && !text.isEmpty())
        Validator::showError(ui->labelError, result.message);
    else
        Validator::hideError(ui->labelError);
}

void AddAnimalDialog::on_btnChooseImage_clicked()
{
    // 1. فتح شاشة اختيار الصورة
    QString sourcePath = QFileDialog::getOpenFileName(this, "Add Image", "", "Images (*.png *.jpg *.jpeg)");

    if (!sourcePath.isEmpty()) {
        // 2. تحديد مسار البرنامج لإنشاء فولدر الصور
        QString appPath = QCoreApplication::applicationDirPath();
        QString imagesFolder = appPath + "/images";

        // 3. لو الفولدر مش موجود، اعمله
        QDir dir;
        if (!dir.exists(imagesFolder)) {
            dir.mkpath(imagesFolder);
        }

        // 4. استخراج اسم الصورة وتجهيز المسار الجديد
        QFileInfo fileInfo(sourcePath);
        QString fileName = fileInfo.fileName();
        QString targetPath = imagesFolder + "/" + fileName;

        // 5. نسخ الصورة (ولو موجودة نمسحها الاول عشان متعملش مشكلة)
        if (QFile::exists(targetPath)) {
            QFile::remove(targetPath);
        }
        QFile::copy(sourcePath, targetPath);

        // 6. حفظ المسار المختصر عشان نبعته للداتابيز
        this->currentImagePath = "images/" + fileName;

        // 7. عرض الصورة في الشاشة
        QPixmap pixmap(targetPath);
        ui->lblImagePreview->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

