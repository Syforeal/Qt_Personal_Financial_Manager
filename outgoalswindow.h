#ifndef OUTGOALSWINDOW_H
#define OUTGOALSWINDOW_H

#include <QMainWindow>
#include<QDate>
#include <QDoubleSpinBox>
namespace Ui {
class outgoalsWindow;
}

class outgoalsWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void recvfinancialgoalsWindow();
    void on_ReturnButton_clicked();
    void onPieSliceHovered(bool status);

    void onSaveBudgetButtonClicked() ;
    void updateBudgetComparison() ;
    void checkBudgetReminders();
    static QDate startOfMonth(const QDate& date) {
        return QDate(date.year(), date.month(), 1);
    }

    static QDate endOfMonth(const QDate& date) {
        return startOfMonth(date).addMonths(1).addDays(-1);
    }
    void loadAccounts();

public:
    explicit outgoalsWindow(QWidget *parent = nullptr);
    ~outgoalsWindow();

private:
    Ui::outgoalsWindow *ui;
    QString currentAccountId;  // 当前选择的账户ID
    QDoubleSpinBox amountbox;

signals:
    void showfinancialgoalswindow();
};

#endif // OUTGOALSWINDOW_H
