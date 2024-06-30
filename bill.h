#ifndef BILL_H
#define BILL_H

#include <QObject>
#include <QDate>

class bill
{
public:
    QString id;
    QString accountId;
    double amount;
    QDate date;
    QString category;
};

#endif // BILL_H
