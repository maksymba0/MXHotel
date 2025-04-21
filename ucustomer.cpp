#include "ucustomer.h"

UCustomer::UCustomer() {}

UCustomer::UCustomer(int ID, const QString &BookingNumber, const QString &Name, int LastRoom, const QString &Phonenumber, const QString &Email, const QString &Information) : ID(ID),
    BookingNumber(BookingNumber),
    Name(Name),
    LastRoom(LastRoom),
    Phonenumber(Phonenumber),
    Email(Email),
    Information(Information)
{}

QString UCustomer::getBookingNumber() const
{
    return BookingNumber;
}

void UCustomer::setBookingNumber(const QString &newBookingNumber)
{
    BookingNumber = newBookingNumber;
}

int UCustomer::getID() const
{
    return ID;
}

void UCustomer::setID(int newID)
{
    ID = newID;
}

QString UCustomer::getName() const
{
    return Name;
}

void UCustomer::setName(const QString &newName)
{
    Name = newName;
}

int UCustomer::getLastRoom() const
{
    return LastRoom;
}

void UCustomer::setLastRoom(int newLastRoom)
{
    LastRoom = newLastRoom;
}

QString UCustomer::getPhonenumber() const
{
    return Phonenumber;
}

void UCustomer::setPhonenumber(const QString &newPhonenumber)
{
    Phonenumber = newPhonenumber;
}

QString UCustomer::getEmail() const
{
    return Email;
}

void UCustomer::setEmail(const QString &newEmail)
{
    Email = newEmail;
}

QString UCustomer::getInformation() const
{
    return Information;
}

void UCustomer::setInformation(const QString &newInformation)
{
    Information = newInformation;
}
