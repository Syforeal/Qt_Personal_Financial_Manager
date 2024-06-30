#ifndef BILLWINDOW_H
#define BILLWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
namespace Ui {
class BillWindow;
}

class BillWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void recvMainWindow();
    void on_ReturnButton_clicked();
    void loadAccounts();
    void loadBills(const QString& accountId);
    void addBill();
    void on_BillTableViewContextMenu_Requested(const QPoint &pos);
    void deleteBill();

public:
    explicit BillWindow(QWidget *parent = nullptr);
    ~BillWindow();

private:
    Ui::BillWindow *ui;
    QString currentAccountId;  // 当前选择的账户ID
    QTableView *billtableView;
    QStandardItemModel *billModel;

signals:
    void showMainWindow();
};

#endif // BILLWINDOW_H
