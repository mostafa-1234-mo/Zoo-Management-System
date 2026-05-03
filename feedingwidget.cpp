#include "feedingwidget.h"
#include "ui_feedingwidget.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QPushButton>
#include <QDebug>
#include <QDateTime>

// ── Constructor ────────────────────────────────────────────────
FeedingWidget::FeedingWidget(AnimalManager* animalManager,
                             DatabaseManager* dbManager,
                             QWidget* parent)
    : QWidget(parent),
    ui(new Ui::FeedingWidget),
    animalManager_(animalManager),
    dbManager_(dbManager)
{
    ui->setupUi(this);

    // ── إعداد الجدول (التعديل لـ 7 أعمدة) ──────────────
    ui->tableFeeding->setColumnCount(7); // 👈 تم التعديل من 6 لـ 7

    QStringList headers;
    // الترتيب هنا لازم يطابق الـ UI اللي في الـ Designer بالظبط
    headers << "الحيوان" << "النوع" << "الطعام"
            << "الحالة" << "آخر تغذية" << "تسجيل" << "اليوم";
    ui->tableFeeding->setHorizontalHeaderLabels(headers);

    ui->tableFeeding->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableFeeding->verticalHeader()
        ->setDefaultSectionSize(45);

    ui->tableFeeding->verticalHeader()->setVisible(false);

    // ── إعداد الـ ProgressBar ──────────────────────
    ui->progressFeeding->setRange(0, 100);
    ui->progressFeeding->setValue(0);

    // ── تنسيق التنبيه ────────────────────────────
    ui->labelAlert->setStyleSheet(
        "color: #a32d2d;"
        "background: #fcebeb;"
        "border-radius: 6px;"
        "padding: 8px;"
        "font-size: 13px;"
        );
    ui->labelAlert->hide();

    // ── ربط الـ Signals ──────────────────────────
    connect(ui->btnResetFeeding, &QPushButton::clicked, this, &FeedingWidget::onResetClicked);

    // ── إعداد الـ Timer للفحص التلقائي ──────────────
    checkTimer_ = new QTimer(this);
    checkTimer_->setInterval(60000); // كل دقيقة
    connect(checkTimer_, &QTimer::timeout, this, &FeedingWidget::onTimerTick);
    checkTimer_->start();

    // تحميل البيانات الأولية
    refreshTable();
}

// ── Destructor ─────────────────────────────────────────────────
FeedingWidget::~FeedingWidget()
{
    if (checkTimer_) checkTimer_->stop();
    delete ui;
}

// ── refreshTable ───────────────────────────────────────────────
void FeedingWidget::refreshTable()
{
    ui->tableFeeding->setRowCount(0);

    QVector<Animal> animals = animalManager_->getAllAnimals();
    ui->tableFeeding->setRowCount(animals.size());

    for (int i = 0; i < animals.size(); ++i) {
        addRowToTable(animals[i], i);
    }

    updateSummary();
    checkAlerts();
}

// ── addRowToTable ──────────────────────────────────────────────
void FeedingWidget::addRowToTable(const Animal& animal, int row)
{
    // الأعمدة النصية (0، 1، 2) كما هي
    ui->tableFeeding->setItem(row, 0, new QTableWidgetItem(animal.getName()));
    ui->tableFeeding->setItem(row, 1, new QTableWidgetItem(animal.getSpecies()));
    ui->tableFeeding->setItem(row, 2, new QTableWidgetItem(animal.getFood()));

    // حالة التغذية (ألوان وحالة) - العمود 3
    QString fedText = animal.getFed() ? "تمت التغذية ✓" : "لم يتغذَّ بعد";
    QTableWidgetItem* fedItem = new QTableWidgetItem(fedText);
    fedItem->setTextAlignment(Qt::AlignCenter);

    if (animal.getFed()) {
        fedItem->setBackground(QColor("#eaf3de"));
        fedItem->setForeground(QColor("#3b6d11"));
    } else {
        fedItem->setBackground(QColor("#faeeda"));
        fedItem->setForeground(QColor("#854f0b"));
    }
    ui->tableFeeding->setItem(row, 3, fedItem);

    // عمود 4: وقت آخر تغذية (ساعة:دقيقة)
    QString lastFedTimeText = (animal.getFed() && animal.getLastFedTime().isValid())
                                  ? animal.getLastFedTime().toString("hh:mm ap")
                                  : "—";
    QTableWidgetItem* timeItem = new QTableWidgetItem(lastFedTimeText);
    timeItem->setTextAlignment(Qt::AlignCenter);
    ui->tableFeeding->setItem(row, 4, timeItem);

    // عمود 5: زرار التسجيل (كما هو)
    QPushButton* btnFeed = new QPushButton();
    if (animal.getFed()) {
        btnFeed->setText("مكتمل");
        btnFeed->setEnabled(false);
        btnFeed->setStyleSheet("background: #f0f0f0; color: #999; border-radius: 4px;");
    } else {
        btnFeed->setText("تسجيل التغذية");
        btnFeed->setStyleSheet("background: #e6f1fb; color: #185fa5; border-radius: 4px; border: 1px solid #b5d4f4;");

        int id = animal.getId();
        connect(btnFeed, &QPushButton::clicked, this, [this, id]() {
            onFeedButtonClicked(id);
        });
    }
    ui->tableFeeding->setCellWidget(row, 5, btnFeed);

    // ── عمود 6: اليوم (yyyy-MM-dd) ──
    QString dateStr = (animal.getFed() && animal.getLastFedTime().isValid())
                          ? animal.getLastFedTime().toString("yyyy-MM-dd")
                          : "—";
    QTableWidgetItem* dateItem = new QTableWidgetItem(dateStr);
    dateItem->setTextAlignment(Qt::AlignCenter);
    ui->tableFeeding->setItem(row, 6, dateItem); // العمود الجديد
}

// ── onFeedButtonClicked ────────────────────────────────────────
void FeedingWidget::onFeedButtonClicked(int animalId)
{
    // 1. تحديث الذاكرة (Manager)
    if (animalManager_->markFed(animalId)) {

        // 2. تحديث قاعدة البيانات فوراً (SQL)
        if (dbManager_ && dbManager_->isConnected()) {
            dbManager_->updateFeedingStatus(animalId, true);
        }

        // 3. تحديث الشاشة
        refreshTable();
        qDebug() << "Saved feeding for ID:" << animalId;
    }
}

// ── onResetClicked ─────────────────────────────────────────────
void FeedingWidget::onResetClicked()
{
    auto reply = QMessageBox::question(this, "يوم جديد؟",
                                       "هل تريد تصفير حالة التغذية لجميع الحيوانات لبدء يوم جديد؟",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    // الحصول على كل الحيوانات وتصفيرهم واحد واحد
    QVector<Animal> animals = animalManager_->getAllAnimals();
    for (int i = 0; i < animals.size(); ++i) {
        int id = animals[i].getId();

        // تصفير في الذاكرة (إحنا هنغير الكائن مباشرة في الـ manager)
        Animal* a = animalManager_->getAnimalById(id);
        if (a) {
            a->setFed(false);

            // تصفير في الداتابيز
            if (dbManager_) {
                dbManager_->updateFeedingStatus(id, false);
            }
        }
    }

    refreshTable();
    QMessageBox::information(this, "تم", "تمت إعادة تعيين الجدول بنجاح!");
}

// ── updateSummary ──────────────────────────────────────────────
void FeedingWidget::updateSummary()
{
    QVector<Animal> animals = animalManager_->getAllAnimals();
    int total = animals.size();
    int fed = 0;
    for (const Animal& a : animals) if (a.getFed()) fed++;

    ui->labelFedSummary->setText(QString("%1 / %2 حيوانات تم إطعامهم").arg(fed).arg(total));

    int percent = (total > 0) ? (fed * 100 / total) : 0;
    ui->progressFeeding->setValue(percent);
}

// ── checkAlerts ────────────────────────────────────────────────
void FeedingWidget::checkAlerts()
{
    QVector<Animal> animals = animalManager_->getAllAnimals();
    QStringList hungry;
    for (const Animal& a : animals) if (!a.getFed()) hungry.append(a.getName());

    if (hungry.isEmpty()) {
        ui->labelAlert->hide();
    } else {
        ui->labelAlert->setText("⚠ تنبيه: لم يتم إطعام ( " + hungry.join(", ") + " )");
        ui->labelAlert->show();
    }
}

void FeedingWidget::onTimerTick() { checkAlerts(); }

void FeedingWidget::setAdminMode(bool isAdmin)
{
    ui->btnResetFeeding->setVisible(isAdmin);
    refreshTable();
}