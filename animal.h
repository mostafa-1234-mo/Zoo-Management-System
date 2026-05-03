#ifndef ANIMAL_H
#define ANIMAL_H

#include <QString>
#include <QDateTime>

enum class HealthStatus {
    OK,
    WARNING,
    CRITICAL
};

class Animal {
private:
    int          id_;
    QString      name_;
    QString      species_;
    int          age_;
    HealthStatus health_;
    QString      food_;
    bool         isFed_;
    QDateTime    lastFedTime_;
    QString imagePath_;


public:
    Animal(int id, const QString& name, const QString& species, int age,
           HealthStatus health, const QString& food,
           bool isFed = false, QDateTime lastFedTime = QDateTime()); // ضفنا دول
    void setImagePath(const QString& path) { imagePath_ = path; }
    QString getImagePath() const { return imagePath_; }
    void setLastFedTime(const QDateTime& time) { lastFedTime_ = time; }

    // Getters
    int          getId()          const;
    QString      getName()        const;
    QString      getSpecies()     const;
    int          getAge()         const;
    HealthStatus getHealth()      const;
    QString      getFood()        const;
    bool         getFed()         const;
    QDateTime    getLastFedTime() const;

    // Setters
    void setHealth(HealthStatus h);
    void setFed(bool fed);
    void setFood(const QString& food);

    // Helpers
    QString healthToString() const;
    QString toQString()      const;
};

#endif // ANIMAL_H