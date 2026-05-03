#include "mainwindow.h"
#include "ui_mainwindow.h"

// ── Qt Core & GUI ──────────────────────────────
#include <QDateTime>
#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QTabWidget>
#include <QStatusBar>
#include <QLabel>

// ── Project Headers ────────────────────────────
#include "animalmanager.h"
#include "ticketmanager.h"
#include "databasemanager.h"
#include "animalwidget.h"
#include "ticketwidget.h"
#include "feedingwidget.h"
#include "dashboardwidget.h"
#include "reportwidget.h"
#include "logger.h"

// ── Constructor ────────────────────────────────────────────────
MainWindow::MainWindow(UserRole role, const QString& username, DatabaseManager* dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentRole_(role)
    , currentUserName_(username)
    , dbManager_(dbManager)
{
    // 1. تهيئة ملف الـ UI (القاعدة الأساسية)
    ui->setupUi(this);

    // 2. إعدادات النافذة (قبل الزحمة)
    setWindowTitle("نظام إدارة حديقة الحيوان");
    setMinimumSize(1100, 720);

    LOG_INFO("جاري تهيئة MainWindow للمستخدم: " + currentUserName_);

    // 3. إنشاء الـ Managers (العقل المحرك)
    animalManager_ = new AnimalManager(this);

    if (dbManager_) {
        ticketManager_ = new TicketManager(dbManager_, this);
        ticketManager_->setCurrentUsername(currentUserName_);
    } else {
        LOG_ERROR("خطأ: DatabaseManager مبعوت Null!");
        ticketManager_ = new TicketManager(nullptr, this);
    }

    // 4. تحميل البيانات (عشان لما الـ Widgets تفتح تلاقي داتا)
    if (dbManager_ && dbManager_->isConnected()) {
        loadDataFromDatabase();
    }

    // 5. إنشاء الـ Widgets (الجسم)
    // ⚠️ ترتيب مهم: لازم الـ Managers يكونوا اتخلقوا واتملوا داتا الأول
    dashboardWidget_ = new DashboardWidget(animalManager_, ticketManager_, this);
    animalWidget_    = new AnimalWidget(animalManager_, dbManager_, this);
    ticketWidget_    = new TicketWidget(ticketManager_, dbManager_, this);
    feedingWidget_   = new FeedingWidget(animalManager_, dbManager_, this);
    reportWidget_    = new ReportWidget(ticketManager_, this);

    // 6. إعداد الـ TabWidget والتبويبات
    tabWidget_ = new QTabWidget(this);

    // ✅ استدعاء الدالة هنا "فقط" (شيلتها من السطر 11 اللي كان عندك)
    // لأن الدالة دي بتستخدم dashboardWidget_.. فلو ناديتها قبل سطر 52 البرنامج هيقفل فوراً
    setupTabsByRole();

    setCentralWidget(tabWidget_);

    // 7. اللمسات الأخيرة
    applyRolePermissions();
    setupStatusBar();

    // 8. الربط البرمجي
    connect(tabWidget_, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // تايمر التحديث
    refreshTimer_ = new QTimer(this);
    refreshTimer_->setInterval(30000);
    connect(refreshTimer_, &QTimer::timeout, this, &MainWindow::onAutoRefresh);
    refreshTimer_->start();

    LOG_INFO("تم تشغيل MainWindow بنجاح.");
}
// دالة مساعدة لتنظيم التبويبات (عشان الكود يكون أنظف)
void MainWindow::setupTabsByRole() {
    if (currentRole_ == UserRole::ADMIN) {
        tabWidget_->addTab(dashboardWidget_, "📊 لوحة التحكم");
        tabWidget_->addTab(animalWidget_,    "🐾 الحيوانات");
        tabWidget_->addTab(feedingWidget_,   "🍏 جدول التغذية");
        tabWidget_->addTab(reportWidget_,    "📄 التقارير");
        setWindowTitle("نظام إدارة حديقة الحيوان — (المدير: " + currentUserName_ + ")");
    } else {
        tabWidget_->addTab(animalWidget_,    "🐾 الحيوانات");
        tabWidget_->addTab(ticketWidget_,    "🎟️ التذاكر والزوار");
        setWindowTitle("نظام إدارة حديقة الحيوان — (الموظف: " + currentUserName_ + ")");
    }
}
// ── Destructor ─────────────────────────────────────────────────
MainWindow::~MainWindow() {
    if (refreshTimer_) refreshTimer_->stop();
    LOG_INFO("تم إغلاق MainWindow للمستخدم: " + currentUserName_);
    delete ui;
}

void MainWindow::loadDataFromDatabase() {
    LOG_INFO("بدء استعادة كل البيانات من القاعدة...");

    // 1. سحب الحيوانات
    QVector<Animal> savedAnimals = dbManager_->loadAllAnimals();
    for (const Animal& a : savedAnimals) {
        animalManager_->addAnimalDirect(a);
    }

    // 2. سحب التذاكر (التعديل الجديد) 🚀
    bool isAdmin = (currentRole_ == UserRole::ADMIN); // بنسأل: هل هو مدير؟
    QVector<Ticket> savedTickets = dbManager_->loadAllTickets(currentUserName_, isAdmin);

    for (const Ticket& t : savedTickets) {
        ticketManager_->addTicketDirect(t);
    }
}
// ── تطبيق الصلاحيات ──────────────────────────────────────────
void MainWindow::applyRolePermissions() {
    bool isAdmin = (currentRole_ == UserRole::ADMIN);
    animalWidget_->setAdminMode(isAdmin);
    feedingWidget_->setAdminMode(isAdmin);
}

// ── إعداد شريط الحالة ─────────────────────────────────────────
void MainWindow::setupStatusBar() {
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet("color: #555; font-size: 12px; padding: 5px;");
    statusBar()->addPermanentWidget(statusLabel_);

    QString roleText = (currentRole_ == UserRole::ADMIN) ? "مدير" : "موظف";

    auto updateTime = [this, roleText]() {
        statusLabel_->setText(QString("👤 المستخدم: %1 | 🔑 الصلاحية: %2 | 🕒 %3")
                                  .arg(currentUserName_)
                                  .arg(roleText)
                                  .arg(QDateTime::currentDateTime().toString("hh:mm:ss AP")));
    };

    updateTime();
    QTimer* clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, updateTime);
    clockTimer->start(1000);
}

// ── التعامل مع تغيير التبويبات ────────────────────────────────
void MainWindow::onTabChanged(int index) {
    QWidget* current = tabWidget_->widget(index);
    if (current == dashboardWidget_) dashboardWidget_->refreshDashboard();
    else if (current == animalWidget_) animalWidget_->refreshTable();
    else if (current == feedingWidget_) feedingWidget_->refreshTable();
    else if (current == reportWidget_) reportWidget_->onRefreshClicked();
    else if (current == ticketWidget_) ticketWidget_->refreshTable();
}

// ── التحديث التلقائي ──────────────────────────────────────────
void MainWindow::onAutoRefresh() {
    if (tabWidget_->currentWidget() == dashboardWidget_) {
        dashboardWidget_->refreshDashboard();
    }
}