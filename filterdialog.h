#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include "transaction.h"
#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QPushButton>

class FilterDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilterDialog(const QStringList &categories, const QVector<Transaction> &transactions, QWidget *parent = nullptr);

    QString getCategory() const;
    QDate getStartDate() const;
    QDate getEndDate() const;
    double getMinAmount() const;
    double getMaxAmount() const;

private:
    QComboBox *categoryComboBox;
    QComboBox *startMonthComboBox;
    QDoubleSpinBox *minAmountSpinBox;
    QDoubleSpinBox *maxAmountSpinBox;
    QPushButton *confirmButton;
    QVector<QDate> availableMonths;
};


#endif // FILTERDIALOG_H
