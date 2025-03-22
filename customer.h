#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <QString>
#include <QDate>

class Customer
{
public:
    Customer();

    bool operator==(const Customer& other) const {
        return name == other.name && age == other.age && email == other.email;
    }

    QString getName() const;

    QString getEmail() const;

    QString getPhonenumber() const;
    void setPhonenumber(const QString &newPhonenumber);

    int getAge() const;

    QDate getDob() const;

    void setName(const QString &newName);

    void setEmail(const QString &newEmail);

    void setAge(int newAge);

    void setDob(const QDate &newDob);

private:
    QString name;
    QString email;
    QString phonenumber;
    int age;
    QDate dob;

};

#endif // CUSTOMER_H
