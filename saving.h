#ifndef SAVING_H
#define SAVING_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDate>

class saving
{

public:
    QString id;
    QString accountId;
    double amount;
    QDate startdate;
    QDate enddate;
    QString category;
};

#endif // SAVING_H
