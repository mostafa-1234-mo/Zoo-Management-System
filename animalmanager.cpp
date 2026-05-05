#include "animalmanager.h"
#include "mainwindow.h"
#include "logger.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QTime>

// ── Constructor ────────────────────────────────────────────────
AnimalManager::AnimalManager(MainWindow *mainWindow) : nextId_(1) {
    ui_window = mainWindow; // حفظ المؤشر للوصول لعناصر الواجهة
}

// ── إضافة حيوان جديد (من الشاشة) ──────────────────────────────
void AnimalManager::addAnimal(const QString& name, const QString& species,
                              int age, HealthStatus health, const QString& food,
                              const QString& imagePath) {

    int currentId = nextId_++; // توليد ID جديد

    // 1. إنشاء الكائن في الذاكرة
    Animal newAnimal(currentId, name, species, age, health, food);
    newAnimal.setImagePath(imagePath);
    animals_.append(newAnimal);

    // 2. الحفظ في قاعدة البيانات
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO animals (id, name, species, age, health, food, is_fed, last_fed, image_path) "
                  "VALUES (:id, :name, :species, :age, :health, :food, 0, '', :image_path)");

    query.bindValue(":id", currentId);
    query.bindValue(":name", name);
    query.bindValue(":species", species);
    query.bindValue(":age", age);
    query.bindValue(":health", newAnimal.healthToString());
    query.bindValue(":food", food);
    query.bindValue(":image_path", imagePath);

    if (!query.exec()) {
        LOG_ERROR("❌ Failed to save to database: " + query.lastError().text());
    } else {
        LOG_INFO("✅ Successfully saved: " + name);
    }
}

// ── إضافة حيوان مباشر (بيستخدم وقت تحميل الداتا من الداتابيز) ────
void AnimalManager::addAnimalDirect(const Animal& animal)
{
    animals_.append(animal);
    // تحديث الـ nextId عشان نضمن إن الـ ID اللي جاي ميتكررش
    if (animal.getId() >= nextId_) {
        nextId_ = animal.getId() + 1;
    }
}

// ── حذف حيوان ──────────────────────────────────────────────────
bool AnimalManager::removeAnimal(int id)
{
    for (int i = 0; i < animals_.size(); ++i) {
        if (animals_[i].getId() == id) {
            QString name = animals_[i].getName();
            animals_.removeAt(i);
            LOG_INFO("تم حذف الحيوان من الذاكرة: " + name + " (ID: " + QString::number(id) + ")");
            return true;
        }
    }
    LOG_WARN("محاولة حذف حيوان غير موجود — ID: " + QString::number(id));
    return false;
}

// ── البحث عن حيوان بالـ ID ─────────────────────────────────────
Animal* AnimalManager::getAnimalById(int id) {
    for (Animal& a : animals_)
        if (a.getId() == id) return &a;
    return nullptr;
}

// ── الحصول على كل الحيوانات ────────────────────────────────────
QVector<Animal> AnimalManager::getAllAnimals() const {
    return animals_;
}

// ── البحث بالاسم ───────────────────────────────────────────────
QVector<Animal> AnimalManager::searchByName(const QString& name) const {
    QVector<Animal> result;
    for (const Animal& a : animals_)
        if (a.getName().contains(name, Qt::CaseInsensitive))
            result.append(a);
    return result;
}

// ── تسجيل التغذية ──────────────────────────────────────────────
bool AnimalManager::markFed(int id)
{
    Animal* a = getAnimalById(id);
    if (!a) return false;

    a->setFed(true);
    a->setLastFedTime(QDateTime::currentDateTime());

    LOG_INFO("تم تسجيل تغذية: " + a->getName());
    return true;
}

// ── الإحصائيات ──────────────────────────────────────────────────
QMap<QString,int> AnimalManager::getStats() const {
    QMap<QString,int> stats;
    stats["total"] = animals_.size();
    stats["needsCare"] = 0;
    QStringList uniqueSpecies;

    for (const Animal& a : animals_) {
        if (a.getHealth() != HealthStatus::OK)
            stats["needsCare"]++;
        if (!uniqueSpecies.contains(a.getSpecies()))
            uniqueSpecies.append(a.getSpecies());
    }
    stats["species"] = uniqueSpecies.size();
    return stats;
}

// ── دوال قاعدة البيانات (Counts) ────────────────────────────────
int AnimalManager::getTotalAnimalsCount() const
{
    QSqlQuery query("SELECT COUNT(*) FROM animals");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

int AnimalManager::getFedAnimalsTodayCount() const
{
    QSqlQuery query;
    // استعلام لجلب عدد الحيوانات التي تم تغذيتها اليوم فقط
    query.prepare("SELECT COUNT(*) FROM animals WHERE is_fed = 1 AND date(last_fed) = date('now', 'localtime')");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int AnimalManager::getLastId() const
{
    if (animals_.isEmpty()) return 0;
    return animals_.last().getId();
}

void AnimalManager::setNextId(int id) {
    nextId_ = id;
}