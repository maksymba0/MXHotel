#ifndef BOOKING_H
#define BOOKING_H
#include <QDateTime>
#include <QString>
#include <QDebug>
#include "customer.h"
class Booking
{
public:
    Booking();
    Booking(const QDateTime &CreatedDate, const QDateTime &CheckedinDate, const QDateTime &CheckoutDate, int RoomNumber, const QString &bookerName, const QString &bookerEmail, const QString &bookerPhonenumber, const QVector<Customer> &customers);
    QDateTime getCreatedDate() const;
    void setCreatedDate(const QDateTime &newCreatedDate);

    QDateTime getCheckedinDate() const;
    void setCheckedinDate(const QDateTime &newCheckedinDate);

    QDateTime getCheckoutDate() const;
    void setCheckoutDate(const QDateTime &newCheckoutDate);

    int getRoomNumber() const;
    void setRoomNumber(int newRoomNumber);

    QString getBookerName() const;
    void setBookerName(const QString &newBookerName);

    QString getBookerEmail() const;
    void setBookerEmail(const QString &newBookerEmail);

    QString getBookerPhonenumber() const;
    void setBookerPhonenumber(const QString &newBookerPhonenumber);

    QVector<Customer> getCustomers() const;
    void setCustomers(const QVector<Customer> &newCustomers);
    void addCustomer(Customer customer);

    QString getBookingNumber() const;
    void setBookingNumber(const QString &newBookingNumber);

protected:
    QDateTime CreatedDate;
    QDateTime CheckedinDate;
    QDateTime CheckoutDate;

    int RoomNumber;

    QString bookingNumber;
    QString bookerName;
    QString bookerEmail;
    QString bookerPhonenumber;


    QVector<Customer> customers;
};

#endif // BOOKING_H
