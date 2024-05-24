#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "account.h"
#include "transaction.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

//新增函数功能：账户、资金流转、导入
private slots:
    void addAccount();
    void addTransaction();
    void loadAccounts();
    void loadTransactions(const QString& accountId);
    void transferFunds();
    void importTransactions();  // 导入交易记录

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString currentAccountId;  // 当前选择的账户ID
    QTableView *transactionTableView;
};
#endif // MAINWINDOW_H
