#ifndef UCUSTOMER_H
#define UCUSTOMER_H
#include <QString>
#include <QDate>

class UCustomer
{
public:
    UCustomer();
    UCustomer(int ID, const QString &BookingNumber, const QString &Name, int LastRoom, const QString &Phonenumber, const QString &Email, const QString &Information);
    bool operator==(const UCustomer& other) const {
        return ID == other.ID;
    }

    QString getBookingNumber() const;
    void setBookingNumber(const QString &newBookingNumber);

    int getID() const;
    void setID(int newID);

    QString getName() const;
    void setName(const QString &newName);

    int getLastRoom() const;
    void setLastRoom(int newLastRoom);

    QString getPhonenumber() const;
    void setPhonenumber(const QString &newPhonenumber);

    QString getEmail() const;
    void setEmail(const QString &newEmail);

    QString getInformation() const;
    void setInformation(const QString &newInformation);

    bool getIsModified() const;
    void setIsModified(bool newIsModified);

    bool getIsBanned() const;
    void setIsBanned(bool newIsBanned);

private:

    int ID;
    QString BookingNumber;
    QString Name;
    int LastRoom;
    QString Phonenumber;
    QString Email;
    QString Information;

    bool isModified;
    bool isBanned;

};

#endif // UCUSTOMER_H
