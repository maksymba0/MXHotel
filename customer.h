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

    QString getName();

    QString getEmail();

    QString getPhonenumber();
    void setPhonenumber(const QString &newPhonenumber);

    int getAge();

    QDate getDob();

    void setName(const QString &newName);

    void setEmail(const QString &newEmail);

    void setAge(int newAge);

    void setDob(const QDate &newDob);

    QString getDocumentType() ;
    void setDocumentType(const QString &newDocumentType);

    QString getDocumentNumber() ;
    void setDocumentNumber(const QString &newDocumentNumber);

    bool getCheckedIn();
    void setCheckedIn(bool newCheckedIn);

private:
    QString name;
    QString email;
    QString phonenumber;
    QString DocumentType;
    QString DocumentNumber;
    int age;
    QDate dob;
    bool checkedIn;

};

#endif // CUSTOMER_H
