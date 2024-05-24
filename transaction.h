#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

class Transaction {
public:
    QString id;
    QString accountId;
    double amount;
    QDate date;
    QString category;
};

#endif // TRANSACTION_H
