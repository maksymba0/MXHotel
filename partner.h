#ifndef PARTNER_H
#define PARTNER_H
#include <QString>
class Partner {
private:
    int id;  // Unique ID
    int partnerType; // 0=taxi, 1=airline, 2=parking, 3=car rental
    QString name;
    QString phoneNumber;
    QString email;
    QString website;
    QString details;
    bool isModified = false; // Optional: useful for tracking changes

public:
    // Constructors
    Partner() = default;
    Partner(int id, int partnerType, const QString& name,
            const QString& phoneNumber, const QString& email,
            const QString& website, const QString& details)
        : id(id), partnerType(partnerType), name(name),
        phoneNumber(phoneNumber), email(email),
        website(website), details(details) {}

    bool operator==(const Partner& other) const
    {
        return id == other.id &&
               partnerType == other.partnerType;
    }
    // Getters
    int getId() const { return id; }
    int getPartnerType() const { return partnerType; }
    QString getName() const { return name; }
    QString getPhoneNumber() const { return phoneNumber; }
    QString getEmail() const { return email; }
    QString getWebsite() const { return website; }
    QString getDetails() const { return details; }
    bool getIsModified() const { return isModified; }

    // Setters
    void setId(int newId) { id = newId; }
    void setPartnerType(int type) { partnerType = type; }
    void setName(const QString& newName) { name = newName; }
    void setPhoneNumber(const QString& phone) { phoneNumber = phone; }
    void setEmail(const QString& mail) { email = mail; }
    void setWebsite(const QString& site) { website = site; }
    void setDetails(const QString& info) { details = info; }
    void setIsModified(bool modified) { isModified = modified; }
};

#endif // PARTNER_H
