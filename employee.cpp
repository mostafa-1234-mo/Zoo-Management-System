#include "employee.h"

Employee::Employee(int id,
                   const QString& name,
                   const QString& role,
                   const QString& username,
                   const QString& password)
    : id_(id),
    name_(name),
    role_(role),
    username_(username),
    password_(password)
{}

int     Employee::getId()       const { return id_; }
QString Employee::getName()     const { return name_; }
QString Employee::getRole()     const { return role_; }
QString Employee::getUsername() const { return username_; }

bool Employee::checkPassword(const QString& input) const {
    return password_ == input;
}