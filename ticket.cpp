#include "ticket.h"

// ── 1. Constructor الافتراضي (مهم جداً لعمليات البحث والقراءة من الداتابيز) ──
Ticket::Ticket()
    : id_(0)
    , visitorName_("")
    , type_(TicketType::ADULT)
    , quantity_(1)
    , totalPrice_(0.0)
{
    saleTime_ = QDateTime::currentDateTime();
}

// ── 2. Constructor ببيانات كاملة (بيُستخدم عند بيع تذكرة جديدة) ──
Ticket::Ticket(int id, const QString& visitorName, TicketType type, int quantity, QDateTime saleTime)
    : id_(id)
    , visitorName_(visitorName)
    , type_(type)
    , quantity_(quantity)
    , saleTime_(saleTime)
{
    // حساب السعر الإجمالي أوتوماتيكياً بناءً على النوع والعدد
    totalPrice_ = getPrice(type_) * quantity_;
}

// ── 3. Getters (لقراءة البيانات وعرضها في الجدول) ─────────────────────────────
int Ticket::getId() const { return id_; }
QString Ticket::getVisitorName() const { return visitorName_; }
TicketType Ticket::getType() const { return type_; }
int Ticket::getQuantity() const { return quantity_; }

// السطر اللي كان فيه المشكلة: اتأكد إنه double
double Ticket::getTotalPrice() const {
    return totalPrice_;
}

QDateTime Ticket::getSaleTime() const { return saleTime_; }
// ── 4. Setters (لتعديل البيانات - دي اللي بتستخدمها الداتابيز وهي بتسحب الداتا) ──
void Ticket::setId(int id) { id_ = id; }
void Ticket::setVisitorName(const QString& name) { visitorName_ = name; }
void Ticket::setType(TicketType type) { type_ = type; }
void Ticket::setQuantity(int quantity) { quantity_ = quantity; }

// السطر التاني اللي كان فيه المشكلة: لازم يستلم double
void Ticket::setTotalPrice(double price) {
    totalPrice_ = price;
}

void Ticket::setSaleTime(const QDateTime& time) { saleTime_ = time; }

// ── 5. Helpers (دوال مساعدة لتنسيق البيانات في الواجهة) ────────────────────────
QString Ticket::getSaleTimeFormatted() const {
    if (!saleTime_.isValid()) return "—";
    // تنسيق الوقت بصيغة (ساعة:دقيقة ص/ق)
    return saleTime_.toString("hh:mm ap");
}

QString Ticket::typeToString() const {
    // نداء الدالة الـ static لتجنب تكرار الكود
    return Ticket::typeToString(type_);
}

// ── 6. Static Functions (منطق الأسعار والترجمة الثابت) ────────────────────────
int Ticket::getPrice(TicketType type) {
    switch (type) {
    case TicketType::ADULT:   return 80;
    case TicketType::CHILD:   return 40;
    case TicketType::FAMILY:  return 200;
    case TicketType::STUDENT: return 50;
    default:                  return 80;
    }
}

QString Ticket::typeToString(TicketType type) {
    switch (type) {
    case TicketType::ADULT:   return "Adult";
    case TicketType::CHILD:   return "Child";
    case TicketType::FAMILY:  return "Family";
    case TicketType::STUDENT: return "Student";
    default:                  return "Unknown";
    }
}