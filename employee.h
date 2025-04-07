#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include <QString>
#include <QDebug>
class Employee
{
public:
    Employee();

    bool operator==(const Employee& other) const
    {
        return other.email == email && other.login == login
        && other.name == name && other.password == password
               && other.phoneNumber == phoneNumber && other.role == role && other.salary == salary;
    }
    Employee& operator=(const Employee& other) {
        if (this != &other) {
            name = other.name;
            role = other.role;
            email = other.email;
            phoneNumber = other.phoneNumber;
            salary = other.salary;
            login = other.login;
            password = other.password;
            IsModified = other.IsModified;
        }
        return *this;
    }
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
        if(getIsModified())
        {
            qDebug() << "*** MODIFIED ***";
        }
        qDebug() << "Name: " << name;
        qDebug() << "Role: " << role;
        qDebug() << "Email: " << email;
        qDebug() << "Phone Number: " << phoneNumber;
        qDebug() << "Salary: " << QString::number(salary, 'f', 2);
        qDebug() << "Login: " << login;
        qDebug() << "Password: " << password;
    }
    bool getIsModified() const;
    void setIsModified(bool newIsModified);

private:
    QString name = "";
    QString role = "";
    QString email = "";
    QString phoneNumber = "";
    float salary = 0.0;
    QString login = "";
    QString password = "";
    bool IsModified = false;
};

#endif // EMPLOYEE_H
