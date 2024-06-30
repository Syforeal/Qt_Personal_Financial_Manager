#ifndef TRANSACTIONWINDOW_H
#define TRANSACTIONWINDOW_H
#include "account.h"
#include <QStandardItemModel>
#include <QMainWindow>
#include <QTableView>

QT_BEGIN_NAMESPACE
namespace Ui {
class TransactionWindow;
}
QT_END_NAMESPACE

class TransactionWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void addAccount();
    void addTransaction();
    void loadAccounts();
    void loadTransactions(const QString& accountId);
    void transferFunds();
    void importTransactions();// 导入交易记录
    void recvMainWindow();
    void on_TransactionReturnButton_clicked();

public:
    TransactionWindow(QWidget *parent = nullptr);


private:
    Ui::TransactionWindow *ui;
    QString currentAccountId;  // 当前选择的账户ID
    QTableView *transactionTableView;

signals:
    void showMainWindow();
};


#endif // TRANSACTIONWINDOW_H
