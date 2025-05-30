#include "employee.h"

Employee::Employee() {}

QString Employee::getName() const
{
    return name;
}

void Employee::setName(const QString &newName)
{
    oldName = name;
    name = newName;
}

QString Employee::getRole() const
{
    return role;
}

void Employee::setRole(const QString &newRole)
{
    role = newRole;
}

QString Employee::getEmail() const
{
    return email;
}

void Employee::setEmail(const QString &newEmail)
{
    oldEmail = email;
    email = newEmail;

}

QString Employee::getPhoneNumber() const
{
    return phoneNumber;
}

void Employee::setPhoneNumber(const QString &newPhoneNumber)
{
    phoneNumber = newPhoneNumber;
}

float Employee::getSalary() const
{
    return salary;
}

void Employee::setSalary(float newSalary)
{
    salary = newSalary;
}

QString Employee::getLogin() const
{
    return login;
}

void Employee::setLogin(const QString &newLogin)
{
    login = newLogin;
}

QString Employee::getPassword() const
{
    return password;
}

void Employee::setPassword(const QString &newPassword)
{
    password = newPassword;
}

bool Employee::getIsModified() const
{
    return IsModified;
}

void Employee::setIsModified(bool newIsModified)
{
    IsModified = newIsModified;
}

QString Employee::getOldEmail() const
{
    return oldEmail;
}

void Employee::setOldEmail(const QString &newOldEmail)
{
    oldEmail = newOldEmail;
}

QString Employee::getOldName() const
{
    return oldName;
}

void Employee::setOldName(const QString &newOldName)
{
    oldName = newOldName;
}


Employee::Employee(const QString &name, const QString &role) : name(name),
    role(role),
    email(name.toLower().replace(" ",".")+"@mxhotel.com")
{}

Employee::Employee(const QString &name, const QString &role, const QString &email, const QString &phoneNumber, float salary, const QString &login, const QString &password) : name(name),
    role(role),
    email(email),
    phoneNumber(phoneNumber),
    salary(salary),
    login(login),
    password(password)
{}
