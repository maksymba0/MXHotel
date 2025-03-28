#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include <QString>
#include <QDebug>
class Employee
{
public:
    Employee();
    Employee(const QString &name, const QString &role);
    QString getName() const;
    void setName(const QString &newName);

    QString getRole() const;
    void setRole(const QString &newRole);

    QString getEmail() const;
    void setEmail(const QString &newEmail);

    QString getPhoneNumber() const;
    void setPhoneNumber(const QString &newPhoneNumber);

    float getSalary() const;
    void setSalary(float newSalary);

    QString getLogin() const;
    void setLogin(const QString &newLogin);

    QString getPassword() const;
    void setPassword(const QString &newPassword);

    void Print() const {
        qDebug() << "Name: " << name;
        qDebug() << "Role: " << role;
        qDebug() << "Email: " << email;
        qDebug() << "Phone Number: " << phoneNumber;
        qDebug() << "Salary: " << QString::number(salary, 'f', 2);
        qDebug() << "Login: " << login;
        qDebug() << "Password: " << password;
    }
private:
    QString name;
    QString role;
    QString email;
    QString phoneNumber;
    float salary;
    QString login;
    QString password;
};

#endif // EMPLOYEE_H
