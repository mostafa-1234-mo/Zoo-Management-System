#include "reportwidget.h"
#include "ui_reportwidget.h"

// ── Important includes to prevent errors ───────────────────────
#include "ticketmanager.h"
#include "ticket.h"

// ── Constructor ────────────────────────────────────────────────
ReportWidget::ReportWidget(TicketManager* ticketManager, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportWidget)
    , ticketManager_(ticketManager)
{
    ui->setupUi(this);

    // 1. Set default dates (from last 30 days until today)
    QDate today = QDate::currentDate();
    ui->dateFrom->setDate(today.addDays(-30)); // Last 30 days
    ui->dateTo->setDate(today);

    // 2. Connect the "Refresh" button
    connect(ui->btnRefresh, &QPushButton::clicked, this, &ReportWidget::onRefreshClicked);

    // 3. Connect the "Filter" button (linked via auto-connection or manual if needed)

    // 4. Update report automatically when changing the report type from the combo box
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

    // ── Case 0: Ticket Revenue Report (Detailed) ──
    if (selectedReport == 0) {
        int totalRevenue = 0;
        for (const Ticket& t : tickets) totalRevenue += t.getTotalPrice();

        reportHtml = "<h2 style='color: #2c3e50; text-align: center;'>Ticket Revenue Report</h2><hr>";
        reportHtml += QString("<h3 style='color: #27ae60; text-align: left;'>💰 Total Revenue: %1 EGP</h3>").arg(totalRevenue);

        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='8' style='border-collapse: collapse; text-align: center; font-family: sans-serif;'>";
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>Ticket ID</th><th>Date</th><th>Type</th><th>Qty</th><th>Value</th><th>Time</th></tr>";

        for (const Ticket& t : tickets) {
            reportHtml += QString("<tr>"
                                  "<td>%1</td>"
                                  "<td style='color: #555;'>%2</td>"
                                  "<td>%3</td>"
                                  "<td>%4</td>"
                                  "<td style='color: #27ae60; font-weight: bold;'>%5 EGP</td>"
                                  "<td style='color: #888;'>%6</td>"
                                  "</tr>")
                              .arg(t.getId())
                              .arg(t.getSaleTime().toString("yyyy-MM-dd"))
                              .arg(t.typeToString())
                              .arg(t.getQuantity())
                              .arg(t.getTotalPrice())
                              .arg(t.getSaleTime().toString("hh:mm ap"));
        }
        reportHtml += "</table>";
    }

    // ── Case 1: Visitor Count Report (By Visitor Name) ──
    else if (selectedReport == 1) {
        int totalVisitors = 0;
        for (const Ticket& t : tickets) totalVisitors += t.getQuantity();

        reportHtml = "<h2 style='color: #2c3e50; text-align: center;'>Visitor Count Report</h2><hr>";
        reportHtml += QString("<h3 style='color: #2980b9; text-align: left;'>👥 Total Actual Visitors: %1</h3>").arg(totalVisitors);

        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='8' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>Ticket ID</th><th>Date</th><th>Visitor Name</th><th>Group Size</th><th>Time</th></tr>";

        for (const Ticket& t : tickets) {
            reportHtml += QString("<tr>"
                                  "<td>%1</td>"
                                  "<td>%2</td>"
                                  "<td>%3</td>"
                                  "<td style='color: #2980b9; font-weight: bold;'>%4</td>"
                                  "<td>%5</td>"
                                  "</tr>")
                              .arg(t.getId())
                              .arg(t.getSaleTime().toString("yyyy-MM-dd"))
                              .arg(t.getVisitorName())
                              .arg(t.getQuantity())
                              .arg(t.getSaleTime().toString("hh:mm ap"));
        }
        reportHtml += "</table>";
    }

    // ── Case 2: Ticket Type Distribution (Statistics) ──
    else if (selectedReport == 2) {
        int adultCount = 0, childCount = 0, familyCount = 0, studentCount = 0;

        for (const Ticket& t : tickets) {
            if (t.getType() == TicketType::ADULT) adultCount += t.getQuantity();
            else if (t.getType() == TicketType::CHILD) childCount += t.getQuantity();
            else if (t.getType() == TicketType::FAMILY) familyCount += t.getQuantity();
            else if (t.getType() == TicketType::STUDENT) studentCount += t.getQuantity();
        }

        reportHtml = "<h2 style='color: #2c3e50; text-align: center;'>Category Distribution Report</h2><hr>";
        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='10' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #e8f6f3;'><th>Ticket Type</th><th>Total Individuals</th></tr>";
        reportHtml += QString("<tr><td>👨 Adult</td><td style='font-weight: bold;'>%1</td></tr>").arg(adultCount);
        reportHtml += QString("<tr><td>👶 Child</td><td style='font-weight: bold;'>%1</td></tr>").arg(childCount);
        reportHtml += QString("<tr><td>👨‍👩‍👧‍👦 Family</td><td style='font-weight: bold;'>%1</td></tr>").arg(familyCount);
        reportHtml += QString("<tr><td>🎓 Student</td><td style='font-weight: bold;'>%1</td></tr>").arg(studentCount);
        reportHtml += "</table>";
    }

    ui->textEdit->setHtml(reportHtml);
}

void ReportWidget::on_btnFilter_clicked()
{
    if (!ticketManager_) return;

    // 1. Retrieve dates from the UI
    QDate from = ui->dateFrom->date();
    QDate to = ui->dateTo->date();

    // 2. Request filtered data from the Manager
    QVector<Ticket> tickets = ticketManager_->getTicketsByDate(from, to);

    // 3. Determine selected report type
    int selectedReport = ui->comboChartType->currentIndex();
    QString reportHtml = "";

    if (selectedReport == 0) {
        int totalRevenue = 0;
        for (const Ticket& t : tickets) totalRevenue += t.getTotalPrice();

        reportHtml = "<h2 style='text-align: center; color: #2c3e50;'>📊 Ticket Revenue Report</h2>";
        reportHtml += "<h3 style='text-align: center;'>Period from: " + from.toString("yyyy-MM-dd") + " to: " + to.toString("yyyy-MM-dd") + "</h3>";
        reportHtml += "<div style='background-color: #27ae60; color: white; padding: 10px; text-align: center; border-radius: 5px; margin-bottom: 10px;'>"
                      "💰 Total Revenue: " + QString::number(totalRevenue) + " EGP</div>";
        reportHtml += buildTableHtml(tickets);
    }
    else if (selectedReport == 1) {
        int totalVisitors = 0;
        for (const Ticket& t : tickets) totalVisitors += t.getQuantity();

        reportHtml = "<h2 style='text-align: center; color: #2980b9;'>👥 Visitor Count Report</h2>";
        reportHtml += "<h3 style='text-align: center;'>Period from: " + from.toString("yyyy-MM-dd") + " to: " + to.toString("yyyy-MM-dd") + "</h3>";
        reportHtml += "<div style='background-color: #2980b9; color: white; padding: 10px; text-align: center; border-radius: 5px; margin-bottom: 10px;'>"
                      "🚶 Total Visitors: " + QString::number(totalVisitors) + "</div>";

        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='5' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>Ticket ID</th><th>Visitor Name</th><th>Group Size</th><th>Time</th></tr>";
        for (const Ticket& t : tickets) {
            reportHtml += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
            .arg(t.getId()).arg(t.getVisitorName()).arg(t.getQuantity()).arg(t.getSaleTimeFormatted());
        }
        reportHtml += "</table>";
    }
    else if (selectedReport == 2) {
        int adult = 0, child = 0, family = 0, student = 0;
        for (const Ticket& t : tickets) {
            if (t.getType() == TicketType::ADULT) adult += t.getQuantity();
            else if (t.getType() == TicketType::CHILD) child += t.getQuantity();
            else if (t.getType() == TicketType::FAMILY) family += t.getQuantity();
            else if (t.getType() == TicketType::STUDENT) student += t.getQuantity();
        }

        reportHtml = "<h2 style='text-align: center; color: #8e44ad;'>🎫 Category Distribution Report</h2>";
        reportHtml += "<h3 style='text-align: center;'>Period from: " + from.toString("yyyy-MM-dd") + " to: " + to.toString("yyyy-MM-dd") + "</h3>";
        reportHtml += "<table width='100%' border='1' cellspacing='0' cellpadding='10' style='border-collapse: collapse; text-align: center;'>";
        reportHtml += "<tr style='background-color: #f2f2f2;'><th>Category</th><th>Total Count</th></tr>";
        reportHtml += "<tr><td>👨 Adults</td><td>" + QString::number(adult) + "</td></tr>";
        reportHtml += "<tr><td>👶 Children</td><td>" + QString::number(child) + "</td></tr>";
        reportHtml += "<tr><td>👨‍👩‍👧‍👦 Families</td><td>" + QString::number(family) + "</td></tr>";
        reportHtml += "<tr><td>🎓 Students</td><td>" + QString::number(student) + "</td></tr>";
        reportHtml += "</table>";
    }

    ui->textEdit->setHtml(reportHtml);
}

QString ReportWidget::buildTableHtml(const QVector<Ticket>& tickets) {
    // 1. Initialize table and format headers
    QString html = "<table width='100%' border='1' cellspacing='0' cellpadding='5' style='border-collapse: collapse; text-align: center;'>";
    html += "<tr style='background-color: #f2f2f2;'><th>Ticket ID</th><th>Type</th><th>Qty</th><th>Value</th><th>Time</th></tr>";

    // 2. Loop through tickets and create table rows
    for (const Ticket& t : tickets) {
        html += "<tr>";
        html += "<td>" + QString::number(t.getId()) + "</td>";
        html += "<td>" + t.typeToString() + "</td>";
        html += "<td>" + QString::number(t.getQuantity()) + "</td>";
        html += "<td style='color: green; font-weight: bold;'>" + QString::number(t.getTotalPrice()) + " EGP</td>";
        html += "<td>" + t.getSaleTimeFormatted() + "</td>";
        html += "</tr>";
    }

    // 3. Close the table
    html += "</table>";

    return html;
}