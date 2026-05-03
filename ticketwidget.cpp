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




// ── Constructor ────────────────────────────────────────────────
TicketWidget::TicketWidget(TicketManager* manager, DatabaseManager* dbManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::TicketWidget), ticketManager_(manager), dbManager_(dbManager)
{
    ui->setupUi(this);

    // ── إعداد جدول التذاكر (تعديل لـ 7 أعمدة) ──────────────
    ui->tableTickets->setColumnCount(7); // 👈 عدلناها من 6 لـ 7

    QStringList headers;
    // الترتيب هنا لازم يطابق الترتيب اللي عملته في الـ Designer بالظبط
    headers << "رقم التذكرة" << "اليوم" << "اسم الزائر" << "النوع"
            << "العدد" << "الإجمالي" << "وقت البيع";
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
    QTableWidgetItem* priceItem = createItem(QString("%L1 ج.م").arg(ticket.getTotalPrice()));
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

    ui->labelTotal->setText(QString("%L1 ج.م").arg(total));
    ui->labelTotal->setStyleSheet("color: #3b6d11; font-weight: bold;");
}

// الدالة دي زي ما هي بتنادي على refreshTable اللي بيحدث البيانات
void TicketWidget::onSellClicked() {
    // 1. 🛡️ صمامات الأمان
    if (!ticketManager_ || !dbManager_) {
        QMessageBox::critical(this, "خطأ فني", "تعذر الوصول لمدير البيانات.");
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
    qDebug() << "🎟️ جاري البيع بواسطة المستخدم:" << (currentUser.isEmpty() ? "مجهول!" : currentUser);

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
        qDebug() << "✅ تم البيع بنجاح";
    } else {
        QMessageBox::warning(this, "فشل الحفظ", "فشل تسجيل التذكرة في قاعدة البيانات.");
    }
}
// 🖨️ دالة الطباعة المحدثة والمؤمنة
void TicketWidget::onPrintClicked() {
    if (ui->tableTickets->rowCount() == 0) {
        QMessageBox::warning(this, "تنبيه", "لا توجد تذاكر لطباعتها!");
        return;
    }

    // 1. اختيار مكان حفظ الملف
    QString fileName = QFileDialog::getSaveFileName(this, "حفظ التذكرة كـ PDF",
                                                    "Ticket_" + ui->tableTickets->item(0, 0)->text(),
                                                    "PDF Files (*.pdf)");

    if (fileName.isEmpty()) return;

    // 2. سحب البيانات من أول صف
    QString id = ui->tableTickets->item(0, 0)->text();
    QString name = ui->tableTickets->item(0, 1)->text();
    QString type = ui->tableTickets->item(0, 2)->text();
    QString price = ui->tableTickets->item(0, 4)->text();
    QString time = ui->tableTickets->item(0, 5)->text();

    // 3. إعداد الـ PDF (هنستخدم HTML عشان التنسيق يبقى سهل وشيك)
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));



    QTextDocument doc;

    // استخدمت pt بدل px عشان مقاس الطباعة يثبت
    QString html = QString(
                       "<div style='direction: rtl; text-align: center; border: 8pt solid #2c3e50; padding: 40pt; font-family: Arial;'>"

                       // عنوان الحديقة - ضخم (60pt)
                       "   <h1 style='color: #2c3e50; font-size: 60pt; margin-bottom: 20pt;'>حديقة الحيوان </h1>"
                       "   <div style='border-top: 4pt solid #2c3e50;'></div>"
                       "   <br><br>"

                       // البيانات الأساسية - كبيرة جداً (30pt)
                       "   <p style='font-size: 30pt; margin: 20pt 0;'><b>رقم التذكرة:</b> %1</p>"
                       "   <p style='font-size: 30pt; margin: 20pt 0;'><b>اسم الزائر :</b> %2</p>"
                       "   <p style='font-size: 30pt; margin: 20pt 0;'><b>نوع التذكرة:</b> %3</p>"
                       "   <br>"

                       // الإجمالي - عملاق (80pt) في برواز أخضر
                       "   <div style='background-color: #f4fce3; padding: 30pt; border: 2pt solid #3b6d11;'>"
                       "       <h2 style='color: #3b6d11; font-size: 80pt; margin: 0;'>الإجمالي: %4</h2>"
                       "   </div>"
                       "   <br><br>"

                       // التذييل
                       "   <p style='color: #666; font-size: 25pt;'>وقت الإصدار: %5</p>"
                       "   <br>"
                       "   <p style='font-size: 30pt; font-weight: bold; color: #2c3e50;'>نتمنى لكم زيارة ممتعة!</p>"
                       "</div>"
                       ).arg(id).arg(name).arg(type).arg(price).arg(time);

    doc.setHtml(html);

    // ضبط حجم المستند ليكون متناسب مع الطابعة
    doc.setPageSize(printer.pageRect(QPrinter::Point).size());
    doc.print(&printer);

    QMessageBox::information(this, "تم", "تم إصدار التذكرة بنجاح بالخط الكبير!");
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
