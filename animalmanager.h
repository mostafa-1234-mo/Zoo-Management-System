#ifndef ANIMALMANAGER_H
#define ANIMALMANAGER_H

#include "animal.h"
#include <QVector>
#include <QMap>

class MainWindow;

class AnimalManager {
private:
    QVector<Animal> animals_;
    int             nextId_;
    MainWindow      *ui_window; // 2. تعريف متغير لحفظ مؤشر الواجهة

public:
    // النسخة الشاملة (سيب دي بس)
    void addAnimal(const QString& name, const QString& species, int age,
                   HealthStatus health, const QString& food, const QString& imagePath = "");

    int getLastId() const;
    int getTotalAnimalsCount() const;
    int getFedAnimalsTodayCount() const;

    // ممتاز، إنت ضفت دي صح زي ما قولنا المرة اللي فاتت
    void addAnimalDirect(const Animal& animal);

    AnimalManager(MainWindow *mainWindow);
    void setNextId(int id);

    // ❌ امسح دالة addAnimal اللي كانت هنا (القديمة)

    bool removeAnimal(int id);
    Animal* getAnimalById(int id);
    QVector<Animal> getAllAnimals() const;
    QVector<Animal> searchByName(const QString& name) const;
    bool markFed(int id);
    QMap<QString, int> getStats() const;

};


#endif // ANIMALMANAGER_H