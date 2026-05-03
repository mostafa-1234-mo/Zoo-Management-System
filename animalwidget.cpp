#include "animalwidget.h"
#include <QtWidgets>
#include <QCoreApplication> // 🔴 السطر ده جديد عشان نقرا مسار البرنامج
#include <QIcon>            // 🔴 السطر ده جديد عشان نعرض الصورة كأيقونة
#include "ui_animalwidget.h"
#include "addanimaldialog.h"
#include <QMessageBox>
#include <QHeaderView>

// ── Constructor ────────────────────────────────────────────────
AnimalWidget::AnimalWidget(AnimalManager* manager, DatabaseManager* dbManager, QWidget* parent)
    : QWidget(parent), ui(new Ui::AnimalWidget), manager_(manager), dbManager_(dbManager)
{
    ui->setupUi(this); // ده بيبني الـ widgets اللي صممناها في الـ Designer

    // ── إعداد الجدول ──────────────────────────────
    // عدد الأعمدة 7 زي ما حددنا في الـ Designer
    ui->tableAnimals->setColumnCount(7);

    // عناوين الأعمدة
    QStringList headers;
    headers << "الرقم" << "الاسم" << "النوع"
            << "العمر" << "الطعام" << "الحالة الصحية" << "آخر تغذية";
    ui->tableAnimals->setHorizontalHeaderLabels(headers);

    // الأعمدة تملا العرض المتاح
    ui->tableAnimals->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    // مسموش للمستخدم يغير ترتيب الأعمدة بالسحب
    ui->tableAnimals->horizontalHeader()
        ->setSectionsMovable(false);

    // ارتفاع الصفوف ثابت 40 pixel
    ui->tableAnimals->verticalHeader()
        ->setDefaultSectionSize(40);

    // مش بنعرض رقم الصف على الشمال
    ui->tableAnimals->verticalHeader()->setVisible(false);

    // ── ربط الـ Signals بالـ Slots ────────────────
    // لما المستخدم يضغط زرار الإضافة → شغّل onAddClicked
    connect(ui->btnAdd,
            &QPushButton::clicked,
            this,
            &AnimalWidget::onAddClicked);

    // لما المستخدم يضغط زرار الحذف → شغّل onDeleteClicked
    connect(ui->btnDelete,
            &QPushButton::clicked,
            this,
            &AnimalWidget::onDeleteClicked);

    // لما النص في خانة البحث يتغير → شغّل onSearchTextChanged
    connect(ui->lineEditSearch,
            &QLineEdit::textChanged,
            this,
            &AnimalWidget::onSearchTextChanged);

    // ── تحميل البيانات الأولية ────────────────────
    refreshTable();
}

// ── Destructor ─────────────────────────────────────────────────
AnimalWidget::~AnimalWidget()
{
    delete ui; // بنمسح الـ UI من الذاكرة
}

// ── refreshTable ───────────────────────────────────────────────
void AnimalWidget::refreshTable()
{
    // امسح كل الصفوف الموجودة في الجدول
    ui->tableAnimals->setRowCount(0);

    // جيب كل الحيوانات من الـ manager
    QVector<Animal> animals = manager_->getAllAnimals();

    // حدد عدد الصفوف بعدد الحيوانات
    ui->tableAnimals->setRowCount(animals.size());

    // لف على كل حيوان وضيفه في الجدول
    for (int i = 0; i < animals.size(); ++i) {
        addRowToTable(animals[i], i);
    }
}

// ── addRowToTable ──────────────────────────────────────────────
void AnimalWidget::addRowToTable(const Animal& animal, int row)
{
    // عمود 0: الرقم
    QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(animal.getId()));
    idItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 0, idItem);

    // عمود 1: الاسم والأيقونة
    QTableWidgetItem* nameItem = new QTableWidgetItem(animal.getName());
    if (!animal.getImagePath().isEmpty()) {
        QString fullPath = QCoreApplication::applicationDirPath() + "/" + animal.getImagePath();
        nameItem->setIcon(QIcon(fullPath));
    }
    nameItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 1, nameItem);
    ui->tableAnimals->setIconSize(QSize(40, 40));

    // عمود 2: النوع
    QTableWidgetItem* speciesItem = new QTableWidgetItem(animal.getSpecies());
    speciesItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 2, speciesItem);

    // عمود 3: العمر
    QTableWidgetItem* ageItem = new QTableWidgetItem(QString::number(animal.getAge()) + " سنوات");
    ageItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 3, ageItem);

    // عمود 4: الطعام
    QTableWidgetItem* foodItem = new QTableWidgetItem(animal.getFood());
    foodItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 4, foodItem);

    // عمود 5: الحالة الصحية
    QTableWidgetItem* healthItem = new QTableWidgetItem(animal.healthToString());
    healthItem->setTextAlignment(Qt::AlignCenter);
    colorHealthCell(healthItem, animal.getHealth());
    ui->tableAnimals->setItem(row, 5, healthItem);

    // ── عمود 6: وقت آخر تغذية (تعديل التنسيق لـ 12 ساعة) ──
    QString fedTimeText = (animal.getFed() && animal.getLastFedTime().isValid())
                              ? animal.getLastFedTime().toString("hh:mm ap") // 👈 التنسيق الموحد
                              : "لم يتغذَّ بعد";
    QTableWidgetItem* timeItem = new QTableWidgetItem(fedTimeText);
    timeItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 6, timeItem);

    // ── عمود 7: اليوم (الجديد لضمان التناسق) ──
    QString fedDateText = (animal.getFed() && animal.getLastFedTime().isValid())
                              ? animal.getLastFedTime().toString("yyyy-MM-dd")
                              : "—";
    QTableWidgetItem* dateItem = new QTableWidgetItem(fedDateText);
    dateItem->setTextAlignment(Qt::AlignCenter);
    ui->tableAnimals->setItem(row, 7, dateItem); // تأكد إنك زودت عدد الأعمدة لـ 8 في الـ Constructor
}

// ── colorHealthCell ────────────────────────────────────────────
void AnimalWidget::colorHealthCell(QTableWidgetItem* item,
                                   HealthStatus health)
{
    switch (health) {
    case HealthStatus::OK:
        item->setBackground(QColor("#eaf3de"));
        item->setForeground(QColor("#3b6d11"));
        break;

    case HealthStatus::WARNING:
        item->setBackground(QColor("#faeeda"));
        item->setForeground(QColor("#854f0b"));
        break;

    case HealthStatus::CRITICAL:
        item->setBackground(QColor("#fcebeb"));
        item->setForeground(QColor("#a32d2d"));
        break;
    }
}

// ── onAddClicked ───────────────────────────────────────────────
void AnimalWidget::onAddClicked()
{
    AddAnimalDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {

        QString name    = dialog.getAnimalName();
        QString species = dialog.getAnimalSpecies();
        int     age     = dialog.getAnimalAge();
        QString food    = dialog.getAnimalFood();
        HealthStatus health = dialog.getAnimalHealth();

        // ── التحقق النهائي قبل الحفظ ──────────────
        auto nameResult    = Validator::validateName(name);
        auto speciesResult = Validator::validateSpecies(species);
        auto ageResult     = Validator::validateAge(age);
        auto foodResult    = Validator::validateFood(food);

        if (!nameResult.isValid) {
            QMessageBox::warning(this, "خطأ", nameResult.message);
            return;
        }
        if (!speciesResult.isValid) {
            QMessageBox::warning(this, "خطأ", speciesResult.message);
            return;
        }
        if (!ageResult.isValid) {
            QMessageBox::warning(this, "خطأ", ageResult.message);
            return;
        }
        if (!foodResult.isValid) {
            QMessageBox::warning(this, "خطأ", foodResult.message);
            return;
        }

        // 🔴 التعديل هنا: بنسحب مسار الصورة من الشاشة ونديه للـ manager
        manager_->addAnimal(name, species, age, health, food, dialog.getImagePath());

        if (dbManager_ && dbManager_->isConnected()) {
            Animal* newAnimal = manager_->getAnimalById(manager_->getLastId());
            if (newAnimal) {
                dbManager_->saveAnimal(*newAnimal);
            }
        }
        refreshTable();
        QMessageBox::information(this, "تم", "تمت إضافة الحيوان بنجاح!");
    }
}

// ── onDeleteClicked ────────────────────────────────────────────
void AnimalWidget::onDeleteClicked()
{
    // جيب الصف اللي المستخدم محدده دلوقتي
    int selectedRow = ui->tableAnimals->currentRow();

    // لو مفيش صف محدد currentRow بترجع -1
    if (selectedRow == -1) {
        QMessageBox::warning(this,
                             "تنبيه",
                             "من فضلك اختار حيوان من الجدول الأول!");
        return;
    }

    // جيب اسم الحيوان من عمود الاسم (عمود رقم 1)
    QString animalName = ui->tableAnimals
                             ->item(selectedRow, 1)
                             ->text();

    // جيب الـ ID من عمود الرقم (عمود رقم 0)
    int animalId = ui->tableAnimals
                       ->item(selectedRow, 0)
                       ->text()
                       .toInt();

    // اسأل المستخدم يأكد الحذف
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "تأكيد الحذف",
        "هل أنت متأكد إنك عايز تحذف " + animalName + "؟",
        QMessageBox::Yes | QMessageBox::No
        );

    // لو المستخدم اختار Yes
    if (reply == QMessageBox::Yes) {
        // احذف من الـ manager (من الذاكرة والشاشة)
        bool removed = manager_->removeAnimal(animalId);

        if (removed) {
            // احذف من قاعدة البيانات (SQLite) عشان ميرجعش تاني!
            if (dbManager_ && dbManager_->isConnected()) {
                dbManager_->deleteAnimal(animalId);
            }

            // حدّث الجدول
            refreshTable();
            QMessageBox::information(this,
                                     "تم",
                                     "تم حذف " + animalName + " بنجاح!");
        } else {
            QMessageBox::critical(this,
                                  "خطأ",
                                  "حدث خطأ أثناء الحذف!");
        }
    }
}

// ── onSearchTextChanged ────────────────────────────────────────
void AnimalWidget::onSearchTextChanged(const QString& text)
{
    // لو خانة البحث فاضية، اعرض كل الحيوانات
    if (text.trimmed().isEmpty()) {
        refreshTable();
        return;
    }

    // ابحث في الـ manager
    QVector<Animal> results = manager_->searchByName(text.trimmed());

    // امسح الجدول الحالي
    ui->tableAnimals->setRowCount(0);
    ui->tableAnimals->setRowCount(results.size());

    // اعرض نتايج البحث
    for (int i = 0; i < results.size(); ++i) {
        addRowToTable(results[i], i);
    }
}

// ── setAdminMode ───────────────────────────────────────────────
void AnimalWidget::setAdminMode(bool isAdmin)
{
    // لو مدير: الزراين شغالين
    // لو مستخدم عادي: الزراين معطلين ومخبيين
    ui->btnAdd->setVisible(isAdmin);
    ui->btnDelete->setVisible(isAdmin);

}