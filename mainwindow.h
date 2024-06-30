#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "account.h"
#include "transaction.h"
#include "filterdialog.h"

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QLabel>

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
    void on_TransactionDetailsButton_clicked();
    void on_FinancialgoalsButton_clicked();
    void on_BillButton_clicked();
    void on_GraphButton_clicked();
    void on_ExitButton_clicked();
    void recvtransactionwindow();
    void recvfinancialgoalswindow();
    void recvbillwindow();
    void recvgraphwindow();

    void addAccount();
    void addTransaction();
    void loadAccounts();
    void loadTransactions(const QString& accountId);
    void loadAccountSummary(const QString& currentAccountId);
    void transferFunds();
    void importTransactions();  // 导入交易记录

    void onAccountSelectionChanged(const QString &accountId);
    //void updateAccount();
    //void updateAccountBalance();
    //void deleteAccount();
    //void updateTransaction();
    //void deleteTransaction();
    void searchTransactions();
    //void showFilterDialog();

    //删改账户、交易记录
    void onAccountListViewContextMenuRequested(const QPoint &pos);
    void onTransactionTableViewContextMenuRequested(const QPoint &pos);
    void editAccount();
    void deleteAccount();
    void editTransaction();
    void deleteTransaction();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



signals:
    void showtransactionwindow();
    void showfinancialgoalswindow();
    void showgraphwindow();
    void showbillwindow();

private:
    Ui::MainWindow *ui;
    QString currentAccountId;  // 当前选择的账户ID
    QLabel *balanceLabel;
    QLabel *incomeLabel;
    QLabel *expenseLabel;
    QTableView *transactionTableView;
    QComboBox *accountComboBox;
    QStandardItemModel *transactionModel;
    QStringList categories;

    QComboBox *startMonthComboBox;
    QVector<QDate> availableMonths;

};
#endif // MAINWINDOW_H
