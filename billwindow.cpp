#include "billwindow.h"
#include "ui_billwindow.h"
#include "api.h"
#include "database.h"
#include <QMessageBox>
#include <QUuid>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStringListModel>
#include <QPushButton>
#include <QToolBar>
#include <QMenu>
#include <QIcon>
#include <QPushButton>
#include <QDate>


BillWindow::BillWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BillWindow)
{
    ui->setupUi(this);
    Database::openDatabase();
    loadAccounts();

    QToolBar *toolBar = addToolBar("ReturnToolBar");
    addToolBar(Qt::LeftToolBarArea,toolBar);

    QPushButton *ReturnButton = new QPushButton(this);
    ReturnButton->setText("返回");

    QIcon con(":/new/button/image/returnbutton.jpg");
    ReturnButton->setIcon(con);

    toolBar->addWidget(ReturnButton);

    connect(ReturnButton, &QPushButton::clicked, this, &BillWindow::on_ReturnButton_clicked);

    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data().toString();
        loadBills(currentAccountId);
    });
    connect(ui->SaveBillButton, &QPushButton::clicked, this, &BillWindow::addBill);

    ui->billtableView->setSelectionMode(QAbstractItemView::ExtendedSelection); //启用多选
    ui->billtableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选择行为为整行选择
    ui->billtableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->billtableView, &QTableView::customContextMenuRequested, this, &BillWindow::on_BillTableViewContextMenu_Requested);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    QIcon con1(":/new/button/image/createbutton.jpg");
    ui->SaveBillButton->setIcon(con1);
    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");
    ui->billtableView->setStyleSheet("QTableView { border-image: url(:/new/background/image/OIP.jpg);  }");
}

BillWindow::~BillWindow()
{
    delete ui;
}

void BillWindow::recvMainWindow()
{
    loadAccounts();
    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data().toString();
        loadBills(currentAccountId);
    });
    billModel=new QStandardItemModel(this);
    ui->billtableView->setModel(billModel);
    this->show();
}

void BillWindow::on_ReturnButton_clicked() // 按钮槽函数
{
    this->hide();            // 隐藏本界面
    emit showMainWindow();    // 激活信号
}

void BillWindow::loadAccounts() {
    QList<Account> accounts = API::getAccounts();
    // Populate the account list view
    // Example with QListView and QStringListModel
    QStringList accountNames;
    for (const Account &account : accounts) {
        accountNames << account.name;
    }
    ui->accountListView->setModel(new QStringListModel(accountNames, this));
}

void BillWindow::loadBills(const QString& accountId) {
    QList<bill> bills = API::getBills(accountId);
    // Populate the transaction table view
    // Example with QTableView and QStandardItemModel
    int sum = 0;
    for (int row = 0; row < bills.size(); row++ ) {
        QDate startDate = QDate::currentDate();
        QDate endDate = bills[row].date;
        if ( startDate.daysTo(endDate) < 0 ) sum += 1;
    }
    QStandardItemModel *model = new QStandardItemModel(bills.size()- sum, 6, this);  // 5 columns: id, accountId, amount, date, category
    model->setHeaderData(0, Qt::Horizontal, tr("AccountId"));
    model->setHeaderData(1, Qt::Horizontal, tr("Amount"));
    model->setHeaderData(2, Qt::Horizontal, tr("Date"));
    model->setHeaderData(3, Qt::Horizontal, tr("Category"));
    model->setHeaderData(4, Qt::Horizontal, tr("Remaining Days"));
    model->setHeaderData(5, Qt::Horizontal, tr("Id"));

    ui->billtableView->setSelectionMode(QAbstractItemView::ExtendedSelection); //启用多选
    ui->billtableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选择行为为整行选择
    ui->billtableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->billtableView, &QTableView::customContextMenuRequested, this, &BillWindow::on_BillTableViewContextMenu_Requested);

    for (int row = 0; row < bills.size()- sum; row++ ) {
        QDate startDate = QDate::currentDate();
        QDate endDate = bills[row].date;
        model->setData(model->index(row, 0), bills[row].accountId);
        model->setData(model->index(row, 1), bills[row].amount);
        model->setData(model->index(row, 2), bills[row].date.toString(Qt::ISODate));
        model->setData(model->index(row, 3), bills[row].category);
        model->setData(model->index(row, 4), startDate.daysTo(endDate) );
        model->setData(model->index(row, 5), bills[row].id);
    }
    ui->billtableView->setModel(model);
    billModel = model;
    ui->billtableView->setColumnHidden(5, true);
}

void BillWindow::addBill() {
    qDebug() << "Current account ID: " << currentAccountId; // 调试输出
    if (currentAccountId.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }

    bool ok;
    double amount = QInputDialog::getDouble(this, tr("Add Bill"),
                                            tr("Amount:"), 0, -100000, 100000, 2, &ok);
    if (ok)
    {
        bill bill;
        bill.id = QUuid::createUuid().toString(); //生成唯一id确保不重复
        bill.accountId = currentAccountId;
        bill.amount = amount;
        bool ookk;
        QString userInput = QInputDialog::getText(this, "Enter Date", "Date (YYYY-MM-DD):", QLineEdit::Normal, QString(), &ookk);
        if (ookk)
        {
            QDate date = QDate::fromString(userInput, "yyyy-MM-dd");
            QDate current = QDate::currentDate();
            if (date.isValid()&&current.daysTo(date) >= 0)
            {
                bill.date = date;
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add bill."));
                ok=false;
            }
        }
        if(ok)
        {
            bill.category = QInputDialog::getText(this, tr("Add Bill"),
                                                     tr("Category:"), QLineEdit::Normal,
                                                          "", &ok);
        }
        if (ok) {
            if (API::addBill(bill))
            {
                QMessageBox::information(this, tr("Success"), tr("Bill added successfully."));
                loadBills(currentAccountId);
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add bill."));
            }
        }
    }
}

void BillWindow::on_BillTableViewContextMenu_Requested(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    QAction action("删除记录", this);

    connect(&action, &QAction::triggered, this, &BillWindow::deleteBill);

    contextMenu.addAction(&action);

    contextMenu.exec(ui->billtableView->viewport()->mapToGlobal(pos));
}

void BillWindow::deleteBill() {
    QModelIndexList selectedIndexes = ui->billtableView->selectionModel()->selectedRows();
    qDebug() << "Selected rows count:" << selectedIndexes.count();  // 调试输出选中行数

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No bill selected."));
        return;
    }
    if (QMessageBox::question(this, tr("Delete Bill"), tr("Are you sure you want to delete the selected bill(s)?")) == QMessageBox::Yes) {
        bool allDeleted = true;

        for (const QModelIndex &index : selectedIndexes) {
            QString billId = billModel->data(billModel->index(index.row(), 5)).toString();
            if (!API::deleteBill(billId)) {
                allDeleted = false;
                QMessageBox::warning(this, tr("Error"), tr("Failed to delete bill with ID: %1").arg(billId));
            }
        }

        if (allDeleted) {
            QMessageBox::information(this, tr("Success"), tr("Selected bill(s) deleted successfully."));
        }

        loadBills(currentAccountId); // 重新加载交易记录
    }
}
