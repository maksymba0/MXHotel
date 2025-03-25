#include "customer.h"

Customer::Customer()
{
    setAge(0);
    setCheckedIn(false);
    setEmail("");
    setPhonenumber("");
    setName("");
    setDocumentNumber("");
}

QString Customer::getName()
{
    return name;
}

QString Customer::getEmail()
{
    return email;
}

QString Customer::getPhonenumber()
{
    return phonenumber;
}

void Customer::setPhonenumber(const QString &newPhonenumber)
{
    phonenumber = newPhonenumber;
}

int Customer::getAge()
{
    return age;
}

QDate Customer::getDob()
{
    return dob;
}

void Customer::setName(const QString &newName)
{
    name = newName;
}

void Customer::setEmail(const QString &newEmail)
{
    email = newEmail;
}

void Customer::setAge(int newAge)
{
    age = newAge;
}

void Customer::setDob(const QDate &newDob)
{
    dob = newDob;
}

QString Customer::getDocumentType()
{
    return DocumentType;
}

void Customer::setDocumentType(const QString &newDocumentType)
{
    DocumentType = newDocumentType;
}

QString Customer::getDocumentNumber()
{
    return DocumentNumber;
}

void Customer::setDocumentNumber(const QString &newDocumentNumber)
{
    DocumentNumber = newDocumentNumber;
}

bool Customer::getCheckedIn()
{
    return checkedIn;
}

void Customer::setCheckedIn(bool newCheckedIn)
{
    checkedIn = newCheckedIn;
}
