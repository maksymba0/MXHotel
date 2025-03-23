#include "customer.h"

Customer::Customer() {}

QString Customer::getName() const
{
    return name;
}

QString Customer::getEmail() const
{
    return email;
}

QString Customer::getPhonenumber() const
{
    return phonenumber;
}

void Customer::setPhonenumber(const QString &newPhonenumber)
{
    phonenumber = newPhonenumber;
}

int Customer::getAge() const
{
    return age;
}

QDate Customer::getDob() const
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

QString Customer::getDocumentType() const
{
    return DocumentType;
}

void Customer::setDocumentType(const QString &newDocumentType)
{
    DocumentType = newDocumentType;
}

QString Customer::getDocumentNumber() const
{
    return DocumentNumber;
}

void Customer::setDocumentNumber(const QString &newDocumentNumber)
{
    DocumentNumber = newDocumentNumber;
}
