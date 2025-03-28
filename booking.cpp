#include "booking.h"
#include "payment.h"
#include <QRegularExpression>
#include <QDateTime>
Booking::Booking()
{
    Clear();
}

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

QVector<Customer>& Booking::getCustomers()
{
    return customers;
}

void Booking::setCustomers(const QVector<Customer> &newCustomers)
{
    customers = newCustomers;
}

Customer *Booking::getCustomerByName(QString Name)
{
    for(auto& customer : customers)
    {
        if(customer.getName() == Name)
        {
            return &customer;
        }
    }
    return nullptr;
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

QVector<Payment>& Booking::getPayments()
{
    return payments;
}

void Booking::setPayments(const QVector<Payment> &newPayments)
{
    payments = newPayments;
}

Payment *Booking::getPayment(Payment& obj)
{
    for(auto& payment : payments)
    {
        if(payment == obj)
        {
            return &payment;
        }
    }
    return nullptr;
}

void Booking::addPayment(Payment payment)
{
    payments.push_back(payment);
    qDebug() << "Added payment;";
}

QString Booking::getNotes() const
{
    return notes;
}

void Booking::AddNote(QString param)
{
    notes.append("\n" + QDateTime::currentDateTime().toString("dd.MM.yyyy | ") + param);
}

void Booking::setNotes(const QString &newNotes)
{
    notes = newNotes;
}

void Booking::Clear()
{
    bookingNumber = "";
    this->setBookerPhonenumber("");
    this->setBookerEmail("");
    this->setBookerName("");
    QDateTime date;
    this->setCheckedinDate(date);
    this->setCheckoutDate(date);
    this->setCreatedDate(QDateTime::currentDateTime());
    this->setRoomNumber(-1);
    this->setNotes("");
    this->getCustomers().erase(getCustomers().begin(),getCustomers().end());
    this->getPayments().clear();

}

void Booking::Print() {
    qDebug() << "CreatedDate: " << CreatedDate.toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "CheckedinDate: " << (CheckedinDate.isValid() ? CheckedinDate.toString("yyyy-MM-dd HH:mm:ss") : "Not set");
    qDebug() << "CheckoutDate: " << (CheckoutDate.isValid() ? CheckoutDate.toString("yyyy-MM-dd HH:mm:ss") : "Not set");

    qDebug() << "RoomNumber: " << RoomNumber;
    qDebug() << "BookingNumber: " << bookingNumber;
    qDebug() << "BookerName: " << bookerName;
    qDebug() << "BookerEmail: " << bookerEmail;
    qDebug() << "BookerPhonenumber: " << bookerPhonenumber;

    qDebug() << "Payments:";
    for (const auto& payment : payments) {
        qDebug() << "  - Amount: " << payment.getAmount(); // Assuming Payment has a getAmount() method
    }

    qDebug() << "Customers:";
    for (auto& customer : customers) {
        qDebug() << "  - Name: " << customer.getName() << ", Age: " << customer.getAge(); // Assuming Customer has getName() and getAge()
    }

    qDebug() << "Notes: " << (!notes.isEmpty() ? notes : "No notes");
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
