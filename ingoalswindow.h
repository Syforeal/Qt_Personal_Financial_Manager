#ifndef INGOALSWINDOW_H
#define INGOALSWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>

namespace Ui {
class ingoalsWindow;
}

class ingoalsWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void recvfinancialgoalsWindow();
    void on_ReturnButton_clicked();
    void loadsavings(const QString& accountId);
    void addsaving();
    void on_SavingTableViewContextMenu_Requested(const QPoint &pos);
    void deleteSaving();
    void showprogress();

public slots:
    void loadAccounts();

public:
    explicit ingoalsWindow(QWidget *parent = nullptr);
    ~ingoalsWindow();

private:
    Ui::ingoalsWindow *ui;
    QString currentAccountId;  // 当前选择的账户ID
    QTableView *savingtableView;
    QStandardItemModel *savingModel;

signals:
    void showfinancialgoalswindow();
};

#endif // INGOALSWINDOW_H
