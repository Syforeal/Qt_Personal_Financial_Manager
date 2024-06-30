#include "ingoalswindow.h"
#include "ui_ingoalswindow.h"
#include "database.h"
#include "api.h"

#include <QMessageBox>
#include <QUuid>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStringListModel>
#include <QPushButton>
#include <QMenu>
#include <QIcon>
#include <QDate>
#include <QToolBar>
ingoalsWindow::ingoalsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ingoalsWindow)
{
    ui->setupUi(this);
    QToolBar *toolBar = addToolBar("ReturnToolBar");
    addToolBar(Qt::LeftToolBarArea,toolBar);

    QPushButton *ReturnButton = new QPushButton(this);
    ReturnButton->setText("返回");

    QIcon con(":/new/button/image/returnbutton.jpg");
    ReturnButton->setIcon(con);

    toolBar->addWidget(ReturnButton);

    Database::openDatabase();
    loadAccounts();

    savingtableView = ui->savingtableView;

    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data(Qt::UserRole + 1).toString();
        loadsavings(currentAccountId);
    });
    connect(ui->SavingsButton, &QPushButton::clicked, this, &ingoalsWindow::addsaving);

    ui->savingtableView->setSelectionMode(QAbstractItemView::ExtendedSelection); //启用多选
    ui->savingtableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选择行为为整行选择
    ui->savingtableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->savingtableView, &QTableView::customContextMenuRequested, this, &ingoalsWindow::on_SavingTableViewContextMenu_Requested);

    connect(ReturnButton, &QPushButton::clicked, this, &ingoalsWindow::on_ReturnButton_clicked);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");
    ui->savingtableView->setStyleSheet("QTableView { border-image: url(:/new/background/image/OIP.jpg);  }");
}

ingoalsWindow::~ingoalsWindow()
{
    delete ui;
}

void ingoalsWindow::recvfinancialgoalsWindow()
{
    this->show(); // 显示本界面
}

void ingoalsWindow::on_ReturnButton_clicked() // 按钮槽函数
{
    this->hide();            // 隐藏本界面
    emit showfinancialgoalswindow();    // 激活信号
}

void ingoalsWindow::loadAccounts() {
    QList<Account> accounts = API::getAccounts();
    QStandardItemModel *model = new QStandardItemModel(accounts.size(), 1, this); // 1 列：用于存储和显示名称
    // 填充模型
    for (int row = 0; row < accounts.size(); ++row) {
        const Account &account = accounts[row];
        QStandardItem *item = new QStandardItem(account.name);
        item->setData(account.id, Qt::UserRole + 1); // 使用 UserRole + 1 存储 ID
        model->setItem(row, 0, item); // 将 item 设置到模型的相应位置
    }

    // 设置模型到 QListView
    ui->accountListView->setModel(model);
}

void ingoalsWindow::loadsavings(const QString& accountId) {
    QList<saving> savings = API::getsavings(accountId);
    qDebug()<<savings.size();

    int sum = 0;
    for (int row = 0; row < savings.size(); row++ ) {
        QDate startDate = QDate::currentDate();
        QDate endDate = savings[row].enddate;
        if ( startDate.daysTo(endDate) < 0 ) sum += 1;
    }
    QStandardItemModel *model = new QStandardItemModel(savings.size()-sum , 7, this);
    model->setHeaderData(0, Qt::Horizontal, tr("AccountName"));
    model->setHeaderData(1, Qt::Horizontal, tr("Amount"));
    model->setHeaderData(2, Qt::Horizontal, tr("StartDate"));
    model->setHeaderData(3, Qt::Horizontal, tr("EndDate"));
    model->setHeaderData(4, Qt::Horizontal, tr("Category"));
    model->setHeaderData(5, Qt::Horizontal, tr("Remaining Days"));
    model->setHeaderData(6, Qt::Horizontal, tr("Id"));

    QString accountName = API::getAccount(accountId).name; //当前选中账户的名称

    for (int row = 0; row < savings.size()-sum; row++ ) {
        QDate startDate = QDate::currentDate();
        QDate endDate = savings[row].enddate;
        model->setData(model->index(row, 0), accountName);
        model->setData(model->index(row, 1), savings[row].amount);
        model->setData(model->index(row, 2), savings[row].startdate.toString(Qt::ISODate));
        model->setData(model->index(row, 3), savings[row].enddate.toString(Qt::ISODate));
        model->setData(model->index(row, 4), savings[row].category);
        model->setData(model->index(row, 5), startDate.daysTo(endDate) );
        model->setData(model->index(row, 6), savings[row].id);
    }
    ui->savingtableView->setModel(model);
    savingModel = model;
    ui->savingtableView->setColumnHidden(6, true);
}

void ingoalsWindow::addsaving() {
    qDebug() << "Current account ID: " << currentAccountId; // 调试输出
    if (currentAccountId.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }
    QList<saving> savings = API::getsavings(currentAccountId);
    if(savings.size()>=3)
    {
        QMessageBox::warning(this, tr("Too many saving plans"), tr("The current account has an excessive number(more than 3) of savings plans.Do not push yourself too hard!"));
        return;
    }
    bool ok;
    double amount = QInputDialog::getDouble(this, tr("Add Saving Plan"),
                                            tr("Amount:"), 0, -100000, 100000, 2, &ok);
    if (ok)
    {
        saving saving;
        saving.id = QUuid::createUuid().toString(); //生成唯一id确保不重复
        saving.accountId = currentAccountId;
        saving.amount = amount;
        saving.startdate= QDate::currentDate();
        bool ookk;
        QString userInput = QInputDialog::getText(this, "Enter Date", "Date (YYYY-MM-DD):", QLineEdit::Normal, QString(), &ookk);
        if (ookk)
        {
            QDate date = QDate::fromString(userInput, "yyyy-MM-dd");
            QDate current = QDate::currentDate();
            if (date.isValid()&&current.daysTo(date) >= 0)
            {
                saving.enddate = date;
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add saving plan."));
                ok=false;
            }
        }
        if(ok)
        {
            saving.category = QInputDialog::getText(this, tr("Add Saving Plan"),
                                                  tr("Category:"), QLineEdit::Normal,
                                                  "", &ok);
        }
        if (ok) {
            if (API::addsaving(saving))
            {
                QMessageBox::information(this, tr("Success"), tr("Saving plan added successfully."));
                loadsavings(currentAccountId);
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add saving plan."));
            }
        }
    }
}

void ingoalsWindow::on_SavingTableViewContextMenu_Requested(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    QAction action1("查看进度（仅单行选中可用）", this);
    QAction action2("删除记录（单、多行均可使用）", this);

    connect(&action1, &QAction::triggered, this, &ingoalsWindow::showprogress);
    connect(&action2, &QAction::triggered, this, &ingoalsWindow::deleteSaving);

    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);

    contextMenu.exec(ui->savingtableView->viewport()->mapToGlobal(pos));
}

void ingoalsWindow::deleteSaving() {
    QModelIndexList selectedIndexes = ui->savingtableView->selectionModel()->selectedRows();
    qDebug() << "Selected rows count:" << selectedIndexes.count();  // 调试输出选中行数

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No saving plan selected."));
        return;
    }
    if (QMessageBox::question(this, tr("Delete Saving Plan"), tr("Are you sure you want to delete the selected saving plan(s)?")) == QMessageBox::Yes) {
        bool allDeleted = true;

        for (const QModelIndex &index : selectedIndexes) {
            QString savingId = savingModel->data(savingModel->index(index.row(), 6)).toString();
            if (!API::deleteSaving(savingId)) {
                allDeleted = false;
                QMessageBox::warning(this, tr("Error"), tr("Failed to delete saving plan with ID: %1").arg(savingId));
            }
        }

        if (allDeleted) {
            QMessageBox::information(this, tr("Success"), tr("Selected saving(s) deleted successfully."));
        }

        loadsavings(currentAccountId);
    }
}

void ingoalsWindow::showprogress()
{
    QModelIndexList selectedIndexes = ui->savingtableView->selectionModel()->selectedRows();

    if(selectedIndexes.size()!=1)
    {
        QMessageBox::warning(this, tr("Error"), tr("Error!"));
        return;
    }
    double amount ;
    QDate startdate ;
    QDate enddate ;
    QDate currentdate=QDate::currentDate() ;
    for (const QModelIndex &index : selectedIndexes) {
        amount = savingModel->data(savingModel->index(index.row(), 1)).toDouble();
        startdate = savingModel->data(savingModel->index(index.row(), 2)).toDate();
        enddate = savingModel->data(savingModel->index(index.row(), 3)).toDate();
    }
    QList<Transaction> transactions = API::getTransactions(currentAccountId);
    double sum = 0; //= Database::updateAccountBalance(accountId, startdate.addDays(-1), enddate);
    for (int row = 0; row < transactions.size(); ++row) {
        double tmp = transactions[row].amount;
        if(startdate.daysTo(transactions[row].date)>=0)  sum += tmp;
    }
    qDebug() << "Current sum: " << sum;
    if(sum-amount<0)
    {
        ui->completionprogressBar->setValue(sum*100/amount);
    }
    else
    {
        ui->completionprogressBar->setValue(100);
    }
    ui->timeprogressBar->setValue(startdate.daysTo(currentdate)*100/startdate.daysTo(enddate));
    if(ui->completionprogressBar->value()==100)
    {
        ui->advice_2->setText("Completed!(Manually cancel this savings plan!)");
    }
    else if(ui->timeprogressBar->value()>=100)
    {
        ui->advice_2->setText("Failed!");
    }
    else if(ui->timeprogressBar->value()<ui->completionprogressBar->value())
    {
        ui->advice_2->setText("Ahead of schedule");
    }
    else if(ui->timeprogressBar->value()==ui->completionprogressBar->value())
    {
        ui->advice_2->setText("On schedule");
    }
    else if(ui->timeprogressBar->value()<ui->completionprogressBar->value())
    {
        ui->advice_2->setText("Behind schedule");
    }
}
