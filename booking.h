#ifndef BOOKING_H
#define BOOKING_H
#include <QDateTime>
#include <QString>
#include <QDebug>
#include "customer.h"
#include "payment.h"
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

    QVector<Customer>& getCustomers();
    void setCustomers(const QVector<Customer> &newCustomers);
    Customer* getCustomerByName(QString Name);
    void addCustomer(Customer customer);

    QString getBookingNumber() const;
    void setBookingNumber(const QString &newBookingNumber);

    QVector<Payment>& getPayments();
    void setPayments(const QVector<Payment> &newPayments);
    Payment* getPayment(Payment&);
    void addPayment(Payment payment);

    QString getNotes() const;
    void AddNote(QString param);
    void setNotes(const QString &newNotes);

    void Clear();

    void Print();
    bool getIsBeingCreated() const;
    void setIsBeingCreated(bool newIsBeingCreated);

    bool getIsModified() const;
    void setIsModified(bool newIsModified);

    bool IsValid()
    {
        return RoomNumber != -1&& // valid room
               bookingNumber != "" && // valid booking number
               !customers.empty(); // valid customer data
    }
    bool getIsChangingRoom() const;
    void setIsChangingRoom(bool newIsChangingRoom);

protected:
    QDateTime CreatedDate;
    QDateTime CheckedinDate;
    QDateTime CheckoutDate;

    int RoomNumber;

    QString bookingNumber;
    QString bookerName;
    QString bookerEmail;
    QString bookerPhonenumber;

    QVector<Payment> payments;
    QVector<Customer> customers;

    QString notes;

    bool isBeingCreated;
    bool IsModified;
    bool IsChangingRoom;
};

#endif // BOOKING_H
