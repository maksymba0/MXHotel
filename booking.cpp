#include "booking.h"

#include <QRegularExpression>

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

void Booking::addCustomer(Customer customer)
{
    if(!customers.contains(customer))
    {
        customers.push_back(customer);
    }

}

QString Booking::getBookingNumber() const
{
    return bookingNumber;
}

void Booking::setBookingNumber(const QString &newBookingNumber)
{
    if(newBookingNumber.size() <= 6 && newBookingNumber.contains(QRegularExpression("^[a-zA-Z0-9]+$")))
    {
        bookingNumber = newBookingNumber;
    }
    else
    {
        qDebug() << "ERROR: RESERVATION NUMBER IS INVALID: " << newBookingNumber;
    }
}

Booking::Booking(const QDateTime &CreatedDate, const QDateTime &CheckedinDate, const QDateTime &CheckoutDate, int RoomNumber, const QString &bookerName, const QString &bookerEmail, const QString &bookerPhonenumber, const QVector<Customer> &customers) : CreatedDate(CreatedDate),
    CheckedinDate(CheckedinDate),
    CheckoutDate(CheckoutDate),
    RoomNumber(RoomNumber),
    bookerName(bookerName),
    bookerEmail(bookerEmail),
    bookerPhonenumber(bookerPhonenumber),
    customers(customers)
{}
