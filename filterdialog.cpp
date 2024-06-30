#include "filterdialog.h"
#include "transaction.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDate>
#include <QSet>
#include <QSqlQuery>>

FilterDialog::FilterDialog(const QStringList &categories, const QVector<Transaction> &transactions, QWidget *parent) : QDialog(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    categoryComboBox = new QComboBox(this);
    categoryComboBox->addItems(categories);
    categoryComboBox->addItem("All Categories");


    startMonthComboBox = new QComboBox(this);
    minAmountSpinBox = new QDoubleSpinBox(this);
    maxAmountSpinBox = new QDoubleSpinBox(this);
    confirmButton = new QPushButton(tr("Filter"), this);
    //filterByValueButton = new QPushButton(tr("Filter by Value"), this);

    // 提取交易记录中所有的月份
    QSet<QDate> months;
    for (const Transaction &transaction : transactions) {
        QDate month = QDate(transaction.date.year(), transaction.date.month(), 1);
        months.insert(month);
    }
    availableMonths = months.values();
    std::sort(availableMonths.begin(), availableMonths.end());

    // 填充 startMonthComboBox
    for (const QDate &month : availableMonths) {
        QString monthString = month.toString("yyyy-MM");
        startMonthComboBox->addItem(monthString, month);
    }

    // 设置默认值为最晚的一个月
    if (!availableMonths.isEmpty()) {
        startMonthComboBox->setCurrentIndex(availableMonths.size() - 1);
    }

    minAmountSpinBox->setRange(-1000000, 1000000);
    maxAmountSpinBox->setRange(-1000000, 1000000);
    // 提取交易记录中最大值和最小值
    double max = 0.0, min = 0.0;
    QSqlQuery query1;
    query1.prepare("SELECT MAX(amount) FROM transactions");
    if (query1.exec() && query1.next()) {
        max = query1.value(0).toDouble();
    }
    QSqlQuery query2;
    query2.prepare("SELECT MIN(amount) FROM transactions");
    if (query2.exec() && query2.next()) {
        min = query2.value(0).toDouble();
    }
    // 设置默认值
    maxAmountSpinBox->setValue(max);
    minAmountSpinBox->setValue(min);


    layout->addWidget(new QLabel(tr("Category:")));
    layout->addWidget(categoryComboBox);
    layout->addWidget(new QLabel(tr("Date:")));
    layout->addWidget(startMonthComboBox);
    layout->addWidget(new QLabel(tr("Min Amount:")));
    layout->addWidget(minAmountSpinBox);
    layout->addWidget(new QLabel(tr("Max Amount:")));
    layout->addWidget(maxAmountSpinBox);
    layout->addWidget(confirmButton);

    setLayout(layout);

    connect(confirmButton, &QPushButton::clicked, this, &QDialog::accept);
}

QString FilterDialog::getCategory() const {
    return categoryComboBox->currentText();
}

QDate FilterDialog::getStartDate() const {
    return startMonthComboBox->currentData().toDate();
}

QDate FilterDialog::getEndDate() const {
    QDate selectedDate = startMonthComboBox->currentData().toDate();
    return QDate(selectedDate.year(), selectedDate.month(), selectedDate.daysInMonth()); // 返回该月的最后一天
}

double FilterDialog::getMinAmount() const {
    return minAmountSpinBox->value();
}

double FilterDialog::getMaxAmount() const {
    return maxAmountSpinBox->value();
}
