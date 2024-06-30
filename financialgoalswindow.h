#ifndef FINANCIALGOALSWINDOW_H
#define FINANCIALGOALSWINDOW_H
#include "mainwindow.h"
#include "api.h"
#include "database.h"
#include <QMainWindow>

namespace Ui {
class financialgoalswindow;
}

class financialgoalswindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void recvMainWindow();
    void recvingoalswindow();
    void recvoutgoalswindow();
    void on_FinancialgoalsReturnButton_clicked();
    void on_ingoalsButton_clicked();
    void on_outgoalsButton_clicked();

public:
    explicit financialgoalswindow(QWidget *parent = nullptr);
    ~financialgoalswindow();

private:
    Ui::financialgoalswindow *ui;
    QString currentAccountId;  // 当前选择的账户ID

signals:
    void showMainWindow();
    void showingoalsWindow();
    void showoutgoalsWindow();
};

#endif // FINANCIALGOALSWINDOW_H
