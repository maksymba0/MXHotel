#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>
class Payment
{
public:
    Payment();
    Payment(float Amount, const QString &Currency, const QString &Method);
    bool operator==(const Payment& other) const
    {
        return Amount == other.Amount && Currency == other.Currency && Method == other.Method;
    }
    QString getCurrency() const;
    void setCurrency(const QString &newCurrency);

    float getAmount() const;
    void setAmount(float newAmount);

    QString getMethod() const;
    void setMethod(const QString &newMethod);

    void PopulateWithRandomValues();

    void Print();
private:
    float Amount;
    QString Currency;
    QString Method;
};

#endif // PAYMENT_H
