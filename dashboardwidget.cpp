#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include "animalmanager.h"
#include "ticketmanager.h"
#include <QDebug>
#include <QHeaderView>

DashboardWidget::DashboardWidget(AnimalManager* am, TicketManager* tm, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DashboardWidget)
    , animalManager_(am)
    , ticketManager_(tm)
{
    // 1. أول وأهم سطر: لازم setupUi تكون رقم واحد قبل أي تعامل مع الـ UI
    ui->setupUi(this);

    // 2. صمام أمان للمديرين
    if (!animalManager_ || !ticketManager_) {
        qDebug() << "Dashboard: Managers are null in constructor!";
    }

    // ── تنسيق كارت الحيوانات — بنفسجي ──────────
    ui->frameAnimals->setStyleSheet(
        "QFrame { background: #EEEDFE; border-radius: 12px; border: 0.5px solid #AFA9EC; padding: 12px; }"
        );
    ui->labelAnimalsNum->setStyleSheet("color: #3C3489; font-size: 36px; font-weight: bold; border: none; background: transparent;");
    ui->labelAnimalsText->setStyleSheet("color: #534AB7; font-size: 13px; border: none; background: transparent;");

    // ── تنسيق كارت الزوار — أزرق ───────────────
    ui->frameVisitors->setStyleSheet(
        "QFrame { background: #E6F1FB; border-radius: 12px; border: 0.5px solid #85B7EB; padding: 12px; }"
        );
    ui->labelVisitorsNum->setStyleSheet("color: #0C447C; font-size: 36px; font-weight: bold; border: none; background: transparent;");
    ui->labelVisitorsText->setStyleSheet("color: #185FA5; font-size: 13px; border: none; background: transparent;");

    // ── تنسيق كارت الإيراد — أخضر ──────────────
    ui->frameRevenue->setStyleSheet(
        "QFrame { background: #EAF3DE; border-radius: 12px; border: 0.5px solid #97C459; padding: 12px; }"
        );
    ui->labelRevenueNum->setStyleSheet("color: #27500A; font-size: 36px; font-weight: bold; border: none; background: transparent;");
    ui->labelRevenueText->setStyleSheet("color: #3B6D11; font-size: 13px; border: none; background: transparent;");

    // ── تنسيق كارت التغذية — برتقالي ────────────
    ui->frameFed->setStyleSheet(
        "QFrame { background: #FAEEDA; border-radius: 12px; border: 0.5px solid #EF9F27; padding: 12px; }"
        );
    ui->labelFedNum->setStyleSheet("color: #633806; font-size: 36px; font-weight: bold; border: none; background: transparent;");
    ui->labelFedText->setStyleSheet("color: #854F0B; font-size: 13px; border: none; background: transparent;");

    // ── إعداد جدول آخر التذاكر ──────────────
    ui->tableRecentTickets->setColumnCount(6);
    QStringList headers;
   headers << "Visitor Name" << "Ticket Type" << "Quantity" << "Total" << "Time" << "Date";
    ui->tableRecentTickets->setHorizontalHeaderLabels(headers);
    ui->tableRecentTickets->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableRecentTickets->verticalHeader()->setVisible(false);
    ui->tableRecentTickets->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableRecentTickets->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableRecentTickets->setAlternatingRowColors(true);

    // 3. تحميل البيانات (مرة واحدة في الآخر بعد ما كل الـ Widgets جهزت)
    refreshDashboard();
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}

void DashboardWidget::refreshDashboard()
{
    if (!ui || !animalManager_ || !ticketManager_) return;

    updateStatCards();
    updateFeedingProgress();
    updateRecentTickets();
}

void DashboardWidget::updateStatCards()
{
    if (!ui || !animalManager_ || !ticketManager_) return;

    qDebug() << "=== Dashboard: Updating Stats ===";

    QVector<Animal> allAnimals = animalManager_->getAllAnimals();
    int totalAnimals = allAnimals.size();
    int totalVisitors = ticketManager_->getTotalVisitors();
    int totalRevenue = ticketManager_->getTotalRevenue();

    // تحديث الأرقام مع التأكد إن الـ Widget موجود (عشان لو غيرت اسمه في الـ Designer)
    if (ui->labelAnimalsNum) ui->labelAnimalsNum->setText(QString::number(totalAnimals));
    if (ui->labelVisitorsNum) ui->labelVisitorsNum->setText(QString::number(totalVisitors));
    if (ui->labelRevenueNum) ui->labelRevenueNum->setText(QString("%L1").arg(totalRevenue));

    int fedCount = 0;
    for (const Animal& a : allAnimals) {
        if (a.getFed()) fedCount++;
    }
    if (ui->labelFedNum) ui->labelFedNum->setText(QString("%1/%2").arg(fedCount).arg(totalAnimals));
}

void DashboardWidget::updateFeedingProgress()
{
    if (!ui || !animalManager_) return;

    QVector<Animal> animals = animalManager_->getAllAnimals();
    int total = animals.size();
    int fedCount = 0;
    for (const Animal& a : animals) {
        if (a.getFed()) fedCount++;
    }

    int percentage = (total > 0) ? (fedCount * 100 / total) : 0;

    ui->progressDashFeeding->setValue(percentage);
    ui->labelFeedingDetail->setText(QString("%1 / %2 animals fed today (%3%)").arg(fedCount).arg(total).arg(percentage));

    // ستايل شريط التقدم
    QString color = (percentage == 100) ? "#639922" : (percentage >= 50 ? "#ef9f27" : "#e24b4a");
    ui->progressDashFeeding->setStyleSheet(QString("QProgressBar::chunk { background: %1; border-radius: 4px; }").arg(color));
}

void DashboardWidget::updateRecentTickets()
{
    if (!ui || !ticketManager_) return;

    ui->tableRecentTickets->setRowCount(0);
    QVector<Ticket> tickets = ticketManager_->getAllTickets();

    if (tickets.isEmpty()) {
        qDebug() << "Dashboard: No tickets to display.";
        return;
    }

    int startIndex = qMax(0, tickets.size() - 5);
    int displayCount = tickets.size() - startIndex;
    ui->tableRecentTickets->setRowCount(displayCount);

    auto makeItem = [](const QString& text) {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        return item;
    };

    int row = 0;
    for (int i = tickets.size() - 1; i >= startIndex; --i, ++row) {
        const Ticket& t = tickets[i];
        ui->tableRecentTickets->setItem(row, 0, makeItem(t.getVisitorName()));
        ui->tableRecentTickets->setItem(row, 1, makeItem(t.typeToString()));
        ui->tableRecentTickets->setItem(row, 2, makeItem(QString::number(t.getQuantity())));

        QTableWidgetItem* priceItem = makeItem(QString("%L1 ج.م").arg(t.getTotalPrice()));
        priceItem->setForeground(QColor("#3b6d11"));
        ui->tableRecentTickets->setItem(row, 3, priceItem);

        QTableWidgetItem* timeItem = makeItem(t.getSaleTimeFormatted());
        timeItem->setForeground(QColor("#888780"));
        ui->tableRecentTickets->setItem(row, 4, timeItem);

        ui->tableRecentTickets->setItem(row, 5, makeItem(t.getSaleTime().toString("yyyy-MM-dd")));
    }
}