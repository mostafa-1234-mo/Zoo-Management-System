#include "reportwidget.h"
#include "ui_reportwidget.h"

// ── الإضافات المهمة عشان مفيش إيرورز تظهر ──
#include "ticketmanager.h"
#include "ticket.h"

// ── Constructor ────────────────────────────────────────────────
ReportWidget::ReportWidget(TicketManager* ticketManager, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportWidget)
    , ticketManager_(ticketManager) // التأكد من استخدام الأقواس () هنا
{
    ui->setupUi(this);

    // 1. تحديد تواريخ افتراضية (من بداية الشهر الحالي لحد النهاردة)
    QDate today = QDate::currentDate();
    ui->dateFrom->setDate(today.addDays(-30)); // آخر 30 يوم
    ui->dateTo->setDate(today);

    // 2. ربط زرار "تحديث" القديم
    connect(ui->btnRefresh, &QPushButton::clicked, this, &ReportWidget::onRefreshClicked);

    // 3. ربط زرار "تصفية" (الفلتر) الجديد بالدالة اللي هنعملها


    // 4. تحديث التقرير تلقائياً لما تغير نوع التقرير من القائمة
    connect(ui->comboChartType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportWidget::onRefreshClicked);



}
// ── Destructor ─────────────────────────────────────────────────
ReportWidget::~ReportWidget()
{
    delete ui;
}

// ── onRefreshClicked ───────────────────────────────────────────
void ReportWidget::onRefreshClicked()
{
    if (!ticketManager_) return;

    QVector<Ticket> tickets = ticketManager_->getAllTickets();
    int selectedReport = ui->comboChartType->currentIndex();
    QString reportHtml = "";

    // ── التقرير الأول: إيراد التذاكر (التفصيلي) ──
    if (selectedReport == 0) {
        int totalRevenue = 0;
        for (const Ticket& t : tickets) totalRevenue += t.getTotalPrice();

        reportHtml = "<h2 style='color: #2c3e50; text-align: center;'>تقرير إيرادات التذاكر</h2><hr>";
        reportHtml += QString("<h3 style='color: #27ae60; text-align: left;'>💰 إجمالي الإيرادات: %1 ج.م</h3>").arg(totalRevenue);

        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='8' style='border-collapse: collapse; text-align: center; font-family: sans-serif;'>";
        // إضافة عمود "اليوم" في الهيدر
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>رقم التذكرة</th><th>اليوم</th><th>النوع</th><th>العدد</th><th>القيمة</th><th>الوقت</th></tr>";

        for (const Ticket& t : tickets) {
            reportHtml += QString("<tr>"
                                  "<td>%1</td>"
                                  "<td style='color: #555;'>%2</td>" // اليوم
                                  "<td>%3</td>"
                                  "<td>%4</td>"
                                  "<td style='color: #27ae60; font-weight: bold;'>%5 ج.م</td>"
                                  "<td style='color: #888;'>%6</td>" // الوقت بتنسيق 12 ساعة
                                  "</tr>")
                                  .arg(t.getId())
                                  .arg(t.getSaleTime().toString("yyyy-MM-dd")) // 👈 إضافة التاريخ
                                  .arg(t.typeToString())
                                  .arg(t.getQuantity())
                                  .arg(t.getTotalPrice())
                                  .arg(t.getSaleTime().toString("hh:mm ap")); // 👈 تنسيق الوقت الموحد
        }
        reportHtml += "</table>";
    }

    // ── التقرير الثاني: عدد الزوار (بأسماء الزوار) ──
    else if (selectedReport == 1) {
        int totalVisitors = 0;
        for (const Ticket& t : tickets) totalVisitors += t.getQuantity();

        reportHtml = "<h2 style='color: #2c3e50; text-align: center;'>تقرير عدد الزوار</h2><hr>";
        reportHtml += QString("<h3 style='color: #2980b9; text-align: left;'>👥 إجمالي الزوار الفعليين: %1 زائر</h3>").arg(totalVisitors);

        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='8' style='border-collapse: collapse; text-align: center;'>";
        // إضافة عمود "اليوم" هنا برضه
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>رقم التذكرة</th><th>اليوم</th><th>اسم الزائر</th><th>العدد المرفق</th><th>الوقت</th></tr>";

        for (const Ticket& t : tickets) {
            reportHtml += QString("<tr>"
                                  "<td>%1</td>"
                                  "<td>%2</td>" // اليوم
                                  "<td>%3</td>"
                                  "<td style='color: #2980b9; font-weight: bold;'>%4</td>"
                                  "<td>%5</td>"
                                  "</tr>")
                                  .arg(t.getId())
                                  .arg(t.getSaleTime().toString("yyyy-MM-dd")) // 👈 إضافة التاريخ
                                  .arg(t.getVisitorName())
                                  .arg(t.getQuantity())
                                  .arg(t.getSaleTime().toString("hh:mm ap"));
        }
        reportHtml += "</table>";
    }

    // ── التقرير الثالث: توزيع أنواع التذاكر (إحصائيات) ──
    else if (selectedReport == 2) {
        int adultCount = 0, childCount = 0, familyCount = 0, studentCount = 0;

        for (const Ticket& t : tickets) {
            if (t.getType() == TicketType::ADULT) adultCount += t.getQuantity();
            else if (t.getType() == TicketType::CHILD) childCount += t.getQuantity();
            else if (t.getType() == TicketType::FAMILY) familyCount += t.getQuantity();
            else if (t.getType() == TicketType::STUDENT) studentCount += t.getQuantity();
        }

        reportHtml = "<h2 style='color: #2c3e50; text-align: center;'>تقرير توزيع الفئات</h2><hr>";
        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='10' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #e8f6f3;'><th>نوع التذكرة</th><th>إجمالي الأفراد</th></tr>";
        reportHtml += QString("<tr><td>👨 بالغ</td><td style='font-weight: bold;'>%1</td></tr>").arg(adultCount);
        reportHtml += QString("<tr><td>👶 طفل</td><td style='font-weight: bold;'>%1</td></tr>").arg(childCount);
        reportHtml += QString("<tr><td>👨‍👩‍👧‍👦 عائلة</td><td style='font-weight: bold;'>%1</td></tr>").arg(familyCount);
        reportHtml += QString("<tr><td>🎓 طالب</td><td style='font-weight: bold;'>%1</td></tr>").arg(studentCount);
        reportHtml += "</table>";
    }

    // عرض التقرير النهائي
    ui->textEdit->setHtml(reportHtml);
}
void ReportWidget::on_btnFilter_clicked()
{
    if (!ticketManager_) return;

    // 1. سحب التواريخ من الواجهة
    QDate from = ui->dateFrom->date();
    QDate to = ui->dateTo->date();

    // 2. طلب البيانات المفلترة من المانجر
    QVector<Ticket> tickets = ticketManager_->getTicketsByDate(from, to);

    // 3. معرفة نوع التقرير المختار (0: إيرادات، 1: زوار، 2: توزيع)
    int selectedReport = ui->comboChartType->currentIndex();
    QString reportHtml = "";

    // ── الحالة الأولى: تقرير إيرادات التذاكر ──
    if (selectedReport == 0) {
        int totalRevenue = 0;
        for (const Ticket& t : tickets) totalRevenue += t.getTotalPrice();

        reportHtml = "<h2 style='text-align: center; color: #2c3e50;'>📊 تقرير إيرادات التذاكر</h2>";
        reportHtml += "<h3 style='text-align: center;'>الفترة من: " + from.toString("yyyy-MM-dd") + " إلى: " + to.toString("yyyy-MM-dd") + "</h3>";
        reportHtml += "<div style='background-color: #27ae60; color: white; padding: 10px; text-align: center; border-radius: 5px; margin-bottom: 10px;'>"
                      "💰 إجمالي الإيرادات: " + QString::number(totalRevenue) + " ج.م</div>";
        reportHtml += buildTableHtml(tickets); // بنستخدم الدالة اللي عملناها لرسم الجدول
    }

    // ── الحالة الثانية: تقرير عدد الزوار ──
    else if (selectedReport == 1) {
        int totalVisitors = 0;
        for (const Ticket& t : tickets) totalVisitors += t.getQuantity();

        reportHtml = "<h2 style='text-align: center; color: #2980b9;'>👥 تقرير عدد الزوار</h2>";
        reportHtml += "<h3 style='text-align: center;'>الفترة من: " + from.toString("yyyy-MM-dd") + " إلى: " + to.toString("yyyy-MM-dd") + "</h3>";
        reportHtml += "<div style='background-color: #2980b9; color: white; padding: 10px; text-align: center; border-radius: 5px; margin-bottom: 10px;'>"
                      "🚶 إجمالي الزوار: " + QString::number(totalVisitors) + " زائر</div>";

        // بناء جدول مخصص للزوار (أسماء الزوار)
        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='5' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>رقم التذكرة</th><th>اسم الزائر</th><th>عدد المرافقين</th><th>الوقت</th></tr>";
        for (const Ticket& t : tickets) {
            reportHtml += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
            .arg(t.getId()).arg(t.getVisitorName()).arg(t.getQuantity()).arg(t.getSaleTimeFormatted());
        }
        reportHtml += "</table>";
    }

    // ── الحالة الثالثة: تقرير توزيع أنواع التذاكر ──
    else if (selectedReport == 2) {
        int adult = 0, child = 0, family = 0, student = 0;
        for (const Ticket& t : tickets) {
            if (t.getType() == TicketType::ADULT) adult += t.getQuantity();
            else if (t.getType() == TicketType::CHILD) child += t.getQuantity();
            else if (t.getType() == TicketType::FAMILY) family += t.getQuantity();
            else if (t.getType() == TicketType::STUDENT) student += t.getQuantity();
        }

        reportHtml = "<h2 style='text-align: center; color: #8e44ad;'>🎫 تقرير توزيع الفئات</h2>";
        reportHtml += "<h3 style='text-align: center;'>الفترة من: " + from.toString("yyyy-MM-dd") + " إلى: " + to.toString("yyyy-MM-dd") + "</h3>";
        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='10' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>الفئة</th><th>العدد الإجمالي</th></tr>";
        reportHtml += "<tr><td>👨 بالغين</td><td>" + QString::number(adult) + "</td></tr>";
        reportHtml += "<tr><td>👶 أطفال</td><td>" + QString::number(child) + "</td></tr>";
        reportHtml += "<tr><td>👨‍👩‍👧‍👦 عائلات</td><td>" + QString::number(family) + "</td></tr>";
        reportHtml += "<tr><td>🎓 طلاب</td><td>" + QString::number(student) + "</td></tr>";
        reportHtml += "</table>";
    }

    // 4. عرض التقرير المختار في الـ textEdit
    ui->textEdit->setHtml(reportHtml);
}
QString ReportWidget::buildTableHtml(const QVector<Ticket>& tickets) {
    // 1. بداية الجدول وتنسيق العناوين
    QString html = "<table width='100%' border='1' cellspacing='0' cellpadding='5' style='border-collapse: collapse; text-align: center;'>";
    html += "<tr style='background-color: #f2f2f2;'><th>رقم التذكرة</th><th>النوع</th><th>العدد</th><th>القيمة</th><th>الوقت</th></tr>";

    // 2. "اللوب" (Loop) - بنمر على كل تذكرة ونرسم لها صف في الجدول
    for (const Ticket& t : tickets) {
        html += "<tr>";
        html += "<td>" + QString::number(t.getId()) + "</td>";
        html += "<td>" + t.typeToString() + "</td>";
        html += "<td>" + QString::number(t.getQuantity()) + "</td>";
        html += "<td style='color: green; font-weight: bold;'>" + QString::number(t.getTotalPrice()) + " ج.م</td>";
        html += "<td>" + t.getSaleTimeFormatted() + "</td>";
        html += "</tr>";
    }

    // 3. قفل الجدول
    html += "</table>";

    return html; // رجع النص الكامل عشان يترسم في الـ textEdit
}

