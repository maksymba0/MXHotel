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

protected:
    QDateTime CreatedDate;
    QDateTime CheckedinDate;
    QDateTime CheckoutDate;
    int RoomNumber;

    QString bookerName;
    QString bookerEmail;
    QString bookerPhonenumber;


    QVector<Customer> customers;
};

#endif // BOOKING_H
