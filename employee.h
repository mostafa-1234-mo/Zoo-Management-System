#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>

class Employee {
private:
    int     id_;
    QString name_;
    QString role_;
    QString username_;
    QString password_;

public:
    Employee(int id,
             const QString& name,
             const QString& role,
             const QString& username,
             const QString& password);

    int     getId()       const;
    QString getName()     const;
    QString getRole()     const;
    QString getUsername() const;

    bool checkPassword(const QString& input) const;
};

#endif // EMPLOYEE_H