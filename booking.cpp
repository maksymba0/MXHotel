#include "booking.h"

Booking::Booking() {}

QDateTime Booking::getCreatedDate() const
{
    return CreatedDate;
}

void Booking::setCreatedDate(const QDateTime &newCreatedDate)
{
    CreatedDate = newCreatedDate;
}

QDateTime Booking::getCheckedinDate() const
{
    return CheckedinDate;
}

void Booking::setCheckedinDate(const QDateTime &newCheckedinDate)
{
    CheckedinDate = newCheckedinDate;
}

QDateTime Booking::getCheckoutDate() const
{
    return CheckoutDate;
}

void Booking::setCheckoutDate(const QDateTime &newCheckoutDate)
{
    CheckoutDate = newCheckoutDate;
}

int Booking::getRoomNumber() const
{
    return RoomNumber;
}

void Booking::setRoomNumber(int newRoomNumber)
{
    RoomNumber = newRoomNumber;
}

QString Booking::getBookerName() const
{
    return bookerName;
}

void Booking::setBookerName(const QString &newBookerName)
{
    bookerName = newBookerName;
}

QString Booking::getBookerEmail() const
{
    return bookerEmail;
}

void Booking::setBookerEmail(const QString &newBookerEmail)
{
    bookerEmail = newBookerEmail;
}

QString Booking::getBookerPhonenumber() const
{
    return bookerPhonenumber;
}

void Booking::setBookerPhonenumber(const QString &newBookerPhonenumber)
{
    bookerPhonenumber = newBookerPhonenumber;
}

QVector<Customer> Booking::getCustomers() const
{
    return customers;
}

void Booking::setCustomers(const QVector<Customer> &newCustomers)
{
    customers = newCustomers;
}
