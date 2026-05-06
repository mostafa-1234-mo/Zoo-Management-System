#include "ticketwidget.h"
#include "ui_ticketwidget.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QStringConverter> // ضروري في Qt 6 للتحويل للعربي
#include "validator.h"
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QDateTime>




// ── Constructor ────────────────────────────────────────────────
TicketWidget::TicketWidget(TicketManager* manager, DatabaseManager* dbManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::TicketWidget), ticketManager_(manager), dbManager_(dbManager)
{
    ui->setupUi(this);

    // ── إعداد جدول التذاكر (تعديل لـ 7 أعمدة) ──────────────
    ui->tableTickets->setColumnCount(7); // 👈 عدلناها من 6 لـ 7

    QStringList headers;
    // الترتيب هنا لازم يطابق الترتيب اللي عملته في الـ Designer بالظبط
    headers << "Ticket ID" << "Date" << "Visitor Name" << "Type"
            << "Quantity" << "Total" << "Sale Time";
    ui->tableTickets->setHorizontalHeaderLabels(headers);

    // جعل الجدول يملأ المساحة المتاحة
    ui->tableTickets->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTickets->verticalHeader()->setDefaultSectionSize(40);
    ui->tableTickets->verticalHeader()->setVisible(false);

    // ── ربط الـ Signals بالـ Slots ────────────────

    // 1. زرار البيع
    connect(ui->btnSell, &QPushButton::clicked, this, &TicketWidget::onSellClicked);

    // 2. تحديث الإجمالي تلقائياً عند تغيير النوع أو العدد
    connect(ui->comboTicketType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TicketWidget::onTicketDetailsChanged);
    connect(ui->spinBoxQty, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TicketWidget::onTicketDetailsChanged);

    // 3. زرار الطباعة
    connect(ui->btnPrintTicket, &QPushButton::clicked, this, &TicketWidget::onPrintClicked);

    // ── تحميل البيانات الأولية ────────────────────
    refreshTable();
}

TicketWidget::~TicketWidget() {
    delete ui;
}


void TicketWidget::refreshTable() {
    if (!ticketManager_) return;

    ui->tableTickets->setRowCount(0); // تصفير الجدول
    QVector<Ticket> tickets = ticketManager_->getAllTickets();

    if (tickets.isEmpty()) {
        return;
    }

    // 👈 السحر هنا: هندور على أحدث تذكرة (صاحبة أكبر ID) في القائمة كلها
    Ticket newestTicket = tickets[0];
    for (int i = 1; i < tickets.size(); ++i) {
        if (tickets[i].getId() > newestTicket.getId()) {
            newestTicket = tickets[i];
        }
    }

    // نفتح صف واحد بس، ونعرض فيه التذكرة الأحدث اللي لقيناها
    ui->tableTickets->insertRow(0);
    addRowToTable(newestTicket, 0);
}
void TicketWidget::addRowToTable(const Ticket& ticket, int row) {
    // 1. 🛡️ صمام أمان: تأكد إن الصف موجود، لو مش موجود بننشئه
    // لو row هو 0 والجدول فاضي، insertRow(0) هيجهز المكان
    if (row >= ui->tableTickets->rowCount()) {
        ui->tableTickets->insertRow(row);
    }

    auto createItem = [](const QString& text) {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        return item;
    };

    // عمود 0: رقم التذكرة
    ui->tableTickets->setItem(row, 0, createItem(QString::number(ticket.getId())));

    // عمود 1: اليوم
    ui->tableTickets->setItem(row, 1, createItem(ticket.getSaleTime().toString("yyyy-MM-dd")));

    // عمود 2: اسم الزائر
    ui->tableTickets->setItem(row, 2, createItem(ticket.getVisitorName()));

    // عمود 3: النوع 👈 (كان فيه غلطة هنا في كودك ومكتوب 4)
    ui->tableTickets->setItem(row, 3, createItem(ticket.typeToString()));

    // عمود 4: العدد 👈 (كان بيمسح اللي قبله)
    ui->tableTickets->setItem(row, 4, createItem(QString::number(ticket.getQuantity())));

    // عمود 5: الإجمالي
    QTableWidgetItem* priceItem = createItem(QString("%L1 EGP").arg(ticket.getTotalPrice()));
    priceItem->setForeground(QColor("#3b6d11")); // اللون الأخضر
    ui->tableTickets->setItem(row, 5, priceItem);

    // عمود 6: وقت البيع
    ui->tableTickets->setItem(row, 6, createItem(ticket.getSaleTimeFormatted()));
}

// الدالة دي زي ما هي مفيش فيها تغيير في المنطق
void TicketWidget::onTicketDetailsChanged() {
    int index = ui->comboTicketType->currentIndex();
    int qty = ui->spinBoxQty->value();
    int total = getPriceByIndex(index) * qty;

    ui->labelTotal->setText(QString("%L1 EGP").arg(total));
    ui->labelTotal->setStyleSheet("color: #3b6d11; font-weight: bold;");
}

// الدالة دي زي ما هي بتنادي على refreshTable اللي بيحدث البيانات
void TicketWidget::onSellClicked() {
    // 1. 🛡️ صمامات الأمان
    if (!ticketManager_ || !dbManager_) {
      QMessageBox::critical(this, "Technical Error", "Unable to access the Database Manager.");
        return;
    }

    QString visitorName = ui->lineEditVisitorName->text().trimmed();
    int qty = ui->spinBoxQty->value();

    // 2. التحقق من صحة الاسم
    auto nameResult = Validator::validateName(visitorName);
    if (!nameResult.isValid) {
        Validator::showError(ui->labelError, nameResult.message);
        return;
    }

    // 3. الحصول على اسم المستخدم الحالي
    QString currentUser = ticketManager_->getCurrentUsername();
    // 🔍 سطر للطباعة عشان نتأكد إن الاسم مش فاضي وقت البيع
    qDebug() << "🎟️ Sale in progress by user:" << (currentUser.isEmpty() ? "Unknown!" : currentUser);

    // 4. إصدار التذكرة برمجياً
    TicketType type = getTypeByIndex(ui->comboTicketType->currentIndex());
    Ticket newTicket = ticketManager_->sellTicket(visitorName, type, qty);

    // 5. محاولة الحفظ في قاعدة البيانات
    bool saveSuccess = false;
    if (dbManager_->isConnected()) {
        saveSuccess = dbManager_->saveTicket(newTicket, currentUser);
    }

    // 6. تحديث الواجهة والمانجر في حالة نجاح الحفظ
    if (saveSuccess) {
        ticketManager_->addTicketDirect(newTicket);
        refreshTable();

        ui->lineEditVisitorName->clear();
        ui->spinBoxQty->setValue(1);
        if(ui->labelError) ui->labelError->clear();

        onTicketDetailsChanged();
        // شلنا الـ QMessageBox عشان متزعجش الموظف مع كل تذكرة بيبيعها (اختياري)
       qDebug() << "✅ Sale completed successfully";
    } else {
       QMessageBox::warning(this, "Save Failed", "Failed to save the ticket to the database.");
    }
}
// 🖨️ دالة الطباعة المحدثة والمؤمنة
void TicketWidget::onPrintClicked() {
    if (ui->tableTickets->rowCount() == 0) {
        QMessageBox::warning(this, "Warning", "No tickets to print!");
        return;
    }

    // 1. Choose where to save the file
    QString fileName = QFileDialog::getSaveFileName(this, "Save Ticket as PDF",
                                                    "Ticket_" + ui->tableTickets->item(0, 0)->text(),
                                                    "PDF Files (*.pdf)");

    if (fileName.isEmpty()) return;

    // 2. Extract data from the first row (with adjusted numbers based on our previous deduction)
    QString id = ui->tableTickets->item(0, 0)->text();    // Ticket ID
    QString name = ui->tableTickets->item(0, 2)->text();  // Name
    QString type = ui->tableTickets->item(0, 3)->text();  // Type (assuming it's in column 3)
    QString price = ui->tableTickets->item(0, 5)->text(); // Total

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm AP");

    // 3. Setup PDF
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    // Reduce margins to expand design area
    printer.setPageMargins(QMarginsF(10, 10, 10, 10));

    QTextDocument doc;

    // 4. Professional HTML design for the ticket
    QString html = QString(
                       "<div style='background-color: #f4f1e1; padding: 20px; font-family: Arial, sans-serif; direction: ltr;'>"

                       // Main ticket container (table to split ticket and verification part)
                       "   <table width='100%' style='border-collapse: collapse; border: 2px solid #2c3e50; background-color: #ffffff;'>"
                       "       <tr>"

                       // ================= Left Section (Main Ticket) =================
                       "           <td width='70%' style='padding: 0; vertical-align: top;'>"

                       // Header (Blue ticket header)
                       "               <div style='background-color: #1a2b40; color: #ffffff; text-align: center; padding: 25px;'>"
                       "                   <h1 style='margin: 0; font-size: 45pt;'>National Zoo</h1>"
                       "                   <p style='margin: 10px 0 0 0; font-size: 20pt; color: #e6e6e6;'>Entry Ticket</p>"
                       "               </div>"

                       // Ticket content
                       "               <div style='padding: 20px;'>"

                       // Ticket data block
                       "                   <h3 style='color: #2c3e50; font-size: 22pt; margin-bottom: 10px; border-bottom: 2px solid #eee; padding-bottom: 5px;'>Ticket Details</h3>"
                       "                   <table width='100%' style='background-color: #f9f9f9; padding: 15px; border: 1px solid #ddd;'>"
                       "                       <tr>"
                       "                           <td width='50%' style='font-size: 18pt; padding: 10px;'><b>Ticket ID:</b> %1</td>"
                       "                           <td width='50%' style='font-size: 18pt; padding: 10px;'><b>Ticket Type:</b> %3</td>"
                       "                       </tr>"
                       "                       <tr>"
                       "                           <td colspan='2' style='font-size: 18pt; padding: 10px;'><b>Visitor Name:</b> %2</td>"
                       "                       </tr>"
                       "                   </table>"
                       "                   <br>"

                       // Visit data block
                       "                   <h3 style='color: #2c3e50; font-size: 22pt; margin-bottom: 10px; border-bottom: 2px solid #eee; padding-bottom: 5px;'>Visit Details</h3>"
                       "                   <div style='background-color: #f9f9f9; padding: 15px; border: 1px solid #ddd; font-size: 18pt;'>"
                       "                       <b>Issue Date and Time:</b> %4"
                       "                   </div>"
                       "                   <br><br>"

                       // Total (Green box)
                       "                   <div style='background-color: #1e4620; color: #ffffff; text-align: center; padding: 25px; border: 4px double #ffffff; outline: 2px solid #1e4620;'>"
                       "                       <h2 style='margin: 0; font-size: 50pt;'>Total: %5</h2>"
                       "                   </div>"

                       // Welcome message
                       "                   <div style='text-align: center; margin-top: 30px;'>"
                       "                       <p style='color: #2c3e50; font-size: 20pt; font-weight: bold;'>Enjoy your visit!</p>"
                       "                   </div>"

                       "               </div>" // End of ticket content
                       "           </td>"

                       // ================= Right Section (Ticket Stub / Verification Part) =================
                       "           <td width='30%' style='border-left: 4px dashed #ccc; padding: 20px; background-color: #f4f1e1; vertical-align: top; text-align: center;'>"
                       "               <br><br>"
                       "               <h3 style='color: #1a2b40; font-size: 20pt; margin-bottom: 30px;'>Verification Stub<br>Entry Ticket</h3>"
                       "               <div style='font-size: 14pt; color: #333; line-height: 2.0; text-align: left; padding-left: 5px;'>"
                       "                   <b>ID:</b> %1<br>"
                       "                   <b>Name:</b> %2<br>"
                       "                   <b>Time:</b> %4"
                       "               </div>"
                       "               <br><br><br><br>"
                       "               <div style='width: 80px; height: 80px; border: 2px solid #1a2b40; margin: 0 auto; display: inline-block;'>"
                       "                   <p style='font-size: 10pt; margin-top: 30px;'>Stamp/QR</p>"
                       "               </div>"
                       "           </td>"

                       "       </tr>"
                       "   </table>"
                       "</div>"
                       ).arg(id).arg(name).arg(type).arg(time).arg(price);

    doc.setHtml(html);

    // Adjust document size to fit the printer
    doc.setPageSize(printer.pageRect(QPrinter::Point).size());
    doc.print(&printer);

    QMessageBox::information(this, "Success", "Ticket issued successfully with the new design!");
}

int TicketWidget::getPriceByIndex(int index) const {
    switch (index) {
    case 0: return 80;  // بالغ
    case 1: return 40;  // طفل
    case 2: return 200; // عائلة
    case 3: return 50;  // طالب
    default: return 80;
    }
}

TicketType TicketWidget::getTypeByIndex(int index) const {
    switch (index) {
    case 0: return TicketType::ADULT;
    case 1: return TicketType::CHILD;
    case 2: return TicketType::FAMILY;
    case 3: return TicketType::STUDENT;
    default: return TicketType::ADULT;
    }
}
