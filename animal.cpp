#include "animal.h"

Animal::Animal(int id, const QString& name, const QString& species, int age,
               HealthStatus health, const QString& food,
               bool isFed, QDateTime lastFedTime)
    : id_(id), name_(name), species_(species), age_(age),
    health_(health), food_(food),
    isFed_(isFed), lastFedTime_(lastFedTime) // بنحفظهم هنا مباشرة
{}

// ── Getters ──────────────────────────────
int          Animal::getId()          const { return id_; }
QString      Animal::getName()        const { return name_; }
QString      Animal::getSpecies()     const { return species_; }
int          Animal::getAge()         const { return age_; }
HealthStatus Animal::getHealth()      const { return health_; }
QString      Animal::getFood()        const { return food_; }
bool         Animal::getFed()         const { return isFed_; }
QDateTime    Animal::getLastFedTime() const { return lastFedTime_; }

// ── Setters ──────────────────────────────
void Animal::setHealth(HealthStatus h) {
    health_ = h;
}

void Animal::setFed(bool fed) {
    isFed_ = fed;
    if (fed)
        lastFedTime_ = QDateTime::currentDateTime();
}

void Animal::setFood(const QString& food) {
    food_ = food;
}

// ── Helpers ──────────────────────────────
QString Animal::healthToString() const {
    switch (health_) {
    case HealthStatus::OK:       return "جيدة";
    case HealthStatus::WARNING:  return "تحتاج متابعة";
    case HealthStatus::CRITICAL: return "حرجة";
    }
    return "جيدة";
}

QString Animal::toQString() const {
    return QString("(%1) %2 — %3 — عمره %4 سنوات — صحته: %5")
        .arg(id_)
        .arg(name_)
        .arg(species_)
        .arg(age_)
        .arg(healthToString());
}