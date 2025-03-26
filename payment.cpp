#include "payment.h"
#include "qdebug.h"
#include <QRandomGenerator>
Payment::Payment() {
    setCurrency(" ");
    setAmount(0.0);
    setMethod(" ");
}

QString Payment::getCurrency() const
{
    return Currency;
}

void Payment::setCurrency(const QString &newCurrency)
{
    Currency = newCurrency;
}

float Payment::getAmount() const
{
    return Amount;
}

void Payment::setAmount(float newAmount)
{
    Amount = newAmount;
}

QString Payment::getMethod() const
{
    return Method;
}

void Payment::setMethod(const QString &newMethod)
{
    Method = newMethod;
}

void Payment::PopulateWithRandomValues()
{
    auto RandomAmount = QRandomGenerator::global()->bounded(1,2000);
    setAmount(RandomAmount);
    auto RandomCurrency = QRandomGenerator::global()->bounded(0,4);
    QString currency;
    switch(RandomCurrency)
    {
    case 0:
        currency = "PLN";
        break;
    case 1:
        currency = "EUR";
        break;
    case 2:
        currency = "UAH";
        break;
    case 3:
        currency = "USD";
        break;
    case 4:
        currency = "GBP";
        break;
    default:
        currency = "PLN";
    }
    setCurrency(currency);

    auto RandomMethod = QRandomGenerator::global()->bounded(0,3);
    QString method;
    switch(RandomMethod)
    {
    case 0:
        method = "VISA";
        break;
    case 1:
        method = "MASTERCARD";
        break;
    case 2:
        method = "GOOGLEPAY";
        break;
    case 3:
    default:
        method = "CASH";
        break;
    }
    setMethod(method);
}

void Payment::Print()
{
    qDebug() << "Payment: "<< getAmount() <<" | " << getCurrency() << " | " << getMethod();
}

Payment::Payment(float Amount, const QString &Currency, const QString &Method) : Amount(Amount),
    Currency(Currency),
    Method(Method)
{}
