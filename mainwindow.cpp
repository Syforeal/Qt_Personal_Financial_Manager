#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filterdialog.h"
#include "api.h"
#include "database.h"

#include <QMenu>
#include <QMessageBox>
#include <QUuid>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStringListModel>
#include <QToolBar>
#include <QPushButton>
#include <QFont>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , transactionModel(new QStandardItemModel(this))  // 初始化 transactionModel
{
    ui->setupUi(this);
    //新增（连接ui）

    Database::openDatabase();
    loadAccounts();
    categories = Database::loadCategories();

    transactionTableView = ui->transactionTableView;


    QToolBar *toolBar = addToolBar("ToolBar");
    addToolBar(Qt::LeftToolBarArea,toolBar);


    QPushButton *FinancialGoalsButton = new QPushButton(this);
    FinancialGoalsButton->setText("财务目标");
    QPushButton *BillButton =new QPushButton(this);
    BillButton->setText("账单提醒");
    QPushButton *GraphButton = new QPushButton(this);
    GraphButton->setText("图表");
    QPushButton *ExitButton = new QPushButton(this);
    ExitButton->setText("退出");

    toolBar->addWidget(FinancialGoalsButton);
    toolBar->addWidget(BillButton);
    toolBar->addWidget(GraphButton);
    toolBar->addWidget(ExitButton);

    connect(FinancialGoalsButton,&QPushButton::clicked,this,&MainWindow::on_FinancialgoalsButton_clicked);
    connect(BillButton,&QPushButton::clicked,this,&MainWindow::on_BillButton_clicked);
    connect(GraphButton,&QPushButton::clicked,this,&MainWindow::on_GraphButton_clicked);
    connect(ExitButton,&QPushButton::clicked,this,&MainWindow::on_ExitButton_clicked);


    // 设置 transactionModel 的列标题
    transactionModel->setColumnCount(5);
    transactionModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    transactionModel->setHeaderData(1, Qt::Horizontal, tr("Account ID"));
    transactionModel->setHeaderData(2, Qt::Horizontal, tr("Amount"));
    transactionModel->setHeaderData(3, Qt::Horizontal, tr("Date"));
    transactionModel->setHeaderData(4, Qt::Horizontal, tr("Category"));
    // transactionModel->setHeaderData(5, Qt::Horizontal, tr("Description"));
    // 将 transactionModel 绑定到 transactionTableView
    ui->transactionTableView->setModel(transactionModel);
    ui->transactionTableView->setColumnHidden(0, true);  // 隐藏ID列
    ui->transactionTableView->setSelectionMode(QAbstractItemView::ExtendedSelection); //启用多选
    ui->transactionTableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选择行为为整行选择

    QToolBar *toolBar_2 = addToolBar("ToolBar");
    addToolBar(Qt::RightToolBarArea,toolBar_2);
    QPushButton *addAccountButton = new QPushButton(this);
    addAccountButton->setText("添加账户");
    QPushButton *addTransactionButton = new QPushButton(this);
    addTransactionButton->setText("添加交易");
    QPushButton *transferFundsButton = new QPushButton(this);
    transferFundsButton->setText("资金转移");
    QPushButton *importTransactionsButton =new QPushButton(this);
    importTransactionsButton->setText("批量导入交易");

    toolBar_2->addWidget(addAccountButton);
    toolBar_2->addWidget(addTransactionButton);
    toolBar_2->addWidget(transferFundsButton);
    toolBar_2->addWidget(importTransactionsButton);

    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::searchTransactions);
    connect(addAccountButton, &QPushButton::clicked, this, &MainWindow::addAccount);
    connect(addTransactionButton, &QPushButton::clicked, this, &MainWindow::addTransaction);
    connect(transferFundsButton, &QPushButton::clicked, this, &MainWindow::transferFunds);
    connect(importTransactionsButton, &QPushButton::clicked, this, &MainWindow::importTransactions);  // 导入交易记录

    QIcon con5(":/new/button/image/addaccountbutton.jpeg");
    addAccountButton->setIcon(con5);

    QIcon con6(":/new/button/image/addtransactionbutton.jpeg");
    addTransactionButton->setIcon(con6);

    QIcon con7(":/new/button/image/transferfundsbutton.jpeg");
    transferFundsButton->setIcon(con7);

    QIcon con8(":/new/button/image/importbutton.jpeg");
    importTransactionsButton->setIcon(con8);

    QIcon con9(":/new/button/image/billbutton.jpeg");
    BillButton->setIcon(con9);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    //按月筛选显示收支
    // 提取交易记录中所有的月份
    QSet<QDate> months;
    QList<Transaction> transactions = API::getAllTransactions();
    for (const Transaction &transaction : transactions) {
        QDate month = QDate(transaction.date.year(), transaction.date.month(), 1);
        months.insert(month);
    }
    availableMonths = months.values();
    std::sort(availableMonths.begin(), availableMonths.end());

    // 填充 startMonthComboBox
    for (const QDate &month : availableMonths) {
        QString monthString = month.toString("yyyy-MM");
        ui -> startMonthComboBox->addItem(monthString, month);
    }

    // 设置默认值为最晚的一个月
    if (!availableMonths.isEmpty()) {
        ui -> startMonthComboBox->setCurrentIndex(availableMonths.size() - 1);
    }

    //连接账户选择与交易记录加载
    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current, const QModelIndex &previous) {
        Q_UNUSED(previous);
        currentAccountId = current.data(Qt::UserRole + 1).toString();
        loadTransactions(currentAccountId);
        loadAccountSummary(currentAccountId);
    });

    // 添加账户列表视图的上下文菜单
    ui->accountListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->accountListView, &QListView::customContextMenuRequested, this, &MainWindow::onAccountListViewContextMenuRequested);

    // 添加交易记录表格视图的上下文菜单
    ui->transactionTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->transactionTableView, &QTableView::customContextMenuRequested, this, &MainWindow::onTransactionTableViewContextMenuRequested);


    QIcon con1(":/new/button/image/exitbutton.jpeg");
    ExitButton->setIcon(con1);

    QIcon con2(":/new/button/image/transactiondetailsbutton.jpeg");
    //TransactionDetailsButton->setIcon(con2);

    QIcon con3(":/new/button/image/financialgoalsbutton.jpeg");
    FinancialGoalsButton->setIcon(con3);

    QIcon con4(":/new/button/image/graphbutton.jpeg");
    GraphButton->setIcon(con4);

    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");
    ui->transactionTableView->setStyleSheet("QTableView { border-image: url(:/new/background/image/OIP.jpg);  }");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ExitButton_clicked()
{
    QApplication *app;
    app->quit();
}

void MainWindow::on_TransactionDetailsButton_clicked() //按钮槽函数
{
    this->hide();            //隐藏本界面
    emit showtransactionwindow();    //激活信号
}

void MainWindow::on_FinancialgoalsButton_clicked() //按钮槽函数
{
    this->hide();            //隐藏本界面
    emit showfinancialgoalswindow();    //激活信号
}

void MainWindow::on_BillButton_clicked() //按钮槽函数
{
    this->hide();            //隐藏本界面
    emit showbillwindow();    //激活信号
}

void MainWindow::on_GraphButton_clicked() //按钮槽函数
{
    this->hide();            //隐藏本界面
    emit showgraphwindow();    //激活信号
}

void MainWindow::recvtransactionwindow()
{
    this->show(); //显示本界面
}

void MainWindow::recvfinancialgoalswindow()
{
    this->show(); //显示本界面
}

void MainWindow::recvbillwindow()
{
    this->show(); //显示本界面
}


void MainWindow::recvgraphwindow()
{
    this->show(); //显示本界面
}


//新增
void MainWindow::loadAccountSummary(const QString& currentAccountId){
    QDate startMonth = ui->startMonthComboBox->currentData().toDate();
    QDate endDate = startMonth.addMonths(1).addDays(-1);
    double monthlyIncome = Database::getTotalIncome(currentAccountId, startMonth, endDate);
    double monthlyExpense = Database::getTotalSpent(currentAccountId, startMonth, endDate);

    ui->incomeLabel->setText(tr("本月收入: %1").arg(monthlyIncome));
    ui->expenseLabel->setText(tr("本月支出: %1").arg(monthlyExpense));
}

void MainWindow::loadAccounts() {
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

void MainWindow::loadTransactions(const QString& accountId) {
    QList<Transaction> transactions = API::getTransactions(accountId);
    // Populate the transaction table view
    // 清空模型中的旧数据
    transactionModel->removeRows(0, transactionModel->rowCount());
    QString name = API::getAccount(accountId).name;
    // 插入新数据
    for (int row = 0; row < transactions.size(); ++row) {
        const Transaction &transaction = transactions[row];
        transactionModel->setItem(row, 0, new QStandardItem(transaction.id));
        transactionModel->setItem(row, 1, new QStandardItem(name));
        transactionModel->setItem(row, 2, new QStandardItem(QString::number(transaction.amount)));
        transactionModel->setItem(row, 3, new QStandardItem(transaction.date.toString(Qt::ISODate)));
        transactionModel->setItem(row, 4, new QStandardItem(transaction.category));
        // transactionModel->setItem(row, 5, new QStandardItem(transaction.description));
    }
    ui->transactionTableView->setModel(transactionModel);
}


void MainWindow::addAccount() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Account"),
                                         tr("Account Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty()) {
        QStringList accountTypes = {"WeChat", "Alipay"};
        QString type = QInputDialog::getItem(this, tr("Account Type"),
                                             tr("Account Type:"), accountTypes, 0, true, &ok);

        if (ok && !type.isEmpty()) {
            Account account;
            account.id = QUuid::createUuid().toString();
            account.name = name;
            account.type = type;
            account.balance = 0.0;
            if (API::addAccount(account)) {
                QMessageBox::information(this, tr("Success"), tr("Account added successfully."));
                loadAccounts();
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add account."));
            }
        }
    }
}

void MainWindow::onAccountSelectionChanged(const QString &accountId) {
    currentAccountId = accountId;
    loadTransactions(currentAccountId);
}

void MainWindow::addTransaction() {
    if (currentAccountId.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }

    bool ok;
    double amount = QInputDialog::getDouble(this, tr("Add Transaction"),
                                            tr("Amount:"), 0, -100000, 100000, 2, &ok);
    if (ok) {
        Transaction transaction;
        transaction.id = QUuid::createUuid().toString(); //生成唯一id确保不重复
        transaction.accountId = currentAccountId;
        transaction.amount = amount;
        transaction.date = QDate::currentDate();
        /*
        transaction.category = QInputDialog::getText(this, tr("Add Transaction"),
                                                     tr("Category:"), QLineEdit::Normal,
                                                     "", &ok);
        */
        // 预设分类项
        QString category = QInputDialog::getItem(this, tr("Add Transaction"),
                                                 tr("Category (select or type new):"), categories, 0, true, &ok);
        if (ok && !category.isEmpty()) {
            if (!categories.contains(category)) {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, tr("Add Custom Category"),
                                              tr("Would you like to add this custom category to the predefined categories?"),
                                              QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    Database::addCustomCategory(category);
                    categories.append(category);
                }
            }
            transaction.category = category;
            //if (ok) {
            if (API::addTransaction(transaction)) {
                QMessageBox::information(this, tr("Success"), tr("Transaction added successfully."));
                loadTransactions(currentAccountId);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add transaction."));
            }
        }
    }
}

void MainWindow::searchTransactions() {
    if (currentAccountId.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }
    QList<Transaction> transactions1 = API::getTransactions(currentAccountId);
    QStringList categories = API::getAllCategories(currentAccountId);
    FilterDialog filterDialog(categories, transactions1, this);
    if (filterDialog.exec() == QDialog::Accepted) {
        QString category = filterDialog.getCategory();
        QDate startDate = filterDialog.getStartDate();
        QDate endDate = filterDialog.getEndDate();
        double minAmount = filterDialog.getMinAmount();
        double maxAmount = filterDialog.getMaxAmount();

        QList<Transaction> transactions = API::searchTransactions(currentAccountId, category, startDate, endDate, minAmount, maxAmount);

        // 清空模型中的旧数据
        transactionModel->removeRows(0, transactionModel->rowCount());

        // 插入新数据
        for (int row = 0; row < transactions.size(); ++row) {
            const Transaction &transaction = transactions[row];
            transactionModel->setItem(row, 0, new QStandardItem(transaction.id));
            transactionModel->setItem(row, 1, new QStandardItem(transaction.accountId));
            transactionModel->setItem(row, 2, new QStandardItem(QString::number(transaction.amount)));
            transactionModel->setItem(row, 3, new QStandardItem(transaction.date.toString(Qt::ISODate)));
            transactionModel->setItem(row, 4, new QStandardItem(transaction.category));
            // transactionModel->setItem(row, 5, new QStandardItem(transaction.description));
        }

        ui->transactionTableView->setModel(transactionModel);
    }
}

void MainWindow::transferFunds() {
    if (currentAccountId.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }

    QStringList accountNames;
    QList<Account> accounts = API::getAccounts();
    for (const Account &account : accounts) {
        if (account.id != currentAccountId) {
            accountNames << account.name;
        }
    }

    bool ok;
    QString toAccountName = QInputDialog::getItem(this, tr("Transfer Funds"),
                                                  tr("To Account:"), accountNames, 0, false, &ok);
    if (ok && !toAccountName.isEmpty()) {
        double amount = QInputDialog::getDouble(this, tr("Transfer Funds"),
                                                tr("Amount:"), 0, 0, 100000, 2, &ok);
        if (ok) {
            QString toAccountId;
            for (const Account &account : accounts) {
                if (account.name == toAccountName) {
                    toAccountId = account.id;
                    break;
                }
            }

            if (API::transferFunds(currentAccountId, toAccountId, amount)) {
                QMessageBox::information(this, tr("Success"), tr("Funds transferred successfully."));
                loadAccounts();
                loadTransactions(currentAccountId);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to transfer funds."));
            }
        }
    }
}

void MainWindow::importTransactions() {
    if (currentAccountId.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, tr("Import Transactions"), "", tr("CSV Files (*.csv)"));
    if (!filePath.isEmpty()) {
        if (API::importTransactionsFromFile(filePath, currentAccountId)) {
            QMessageBox::information(this, tr("Success"), tr("Transactions imported successfully."));
            loadTransactions(currentAccountId);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to import transactions."));
        }
    }
}

// 添加右键菜单到交易记录表格视图
void MainWindow::onTransactionTableViewContextMenuRequested(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    QAction action1("保存(多条)编辑", this);
    QAction action2("删除（多条）记录", this);

    connect(&action1, &QAction::triggered, this, &MainWindow::editTransaction);
    connect(&action2, &QAction::triggered, this, &MainWindow::deleteTransaction);

    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);

    contextMenu.exec(ui->transactionTableView->viewport()->mapToGlobal(pos));
}
//编辑交易记录
void MainWindow::editTransaction() {
    QModelIndexList selectedIndexes = ui->transactionTableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No transaction selected."));
        return;
    }

    bool all = true;

    for (const QModelIndex &index : selectedIndexes) {
        // 获取编辑后的数据
        Transaction transaction;
        transaction.id = ui->transactionTableView->model()->data(ui->transactionTableView->model()->index(index.row(), 0), Qt::DisplayRole).toString();
        transaction.accountId = ui->transactionTableView->model()->data(ui->transactionTableView->model()->index(index.row(), 1)).toString();
        transaction.amount = ui->transactionTableView->model()->data(ui->transactionTableView->model()->index(index.row(), 2)).toDouble();
        transaction.date = QDate::fromString(ui->transactionTableView->model()->data(ui->transactionTableView->model()->index(index.row(), 3)).toString(), Qt::ISODate);
        transaction.category = ui->transactionTableView->model()->data(ui->transactionTableView->model()->index(index.row(), 4)).toString();
        // QString description = ui->transactionTableView->model()->data(ui->transactionTableView->model()->index(index.row(), 5)).toString();
        if (!API::updateTransaction(transaction)) {
            all = false;
            QMessageBox::warning(this, tr("Error"), tr("Failed to update transaction with ID: %1").arg(transaction.id));
        }
    }
    if (all) {
        QMessageBox::information(this, tr("Success"), tr("Selected transaction(s) deleted successfully."));
    }
    loadTransactions(currentAccountId); // 重新加载交易记录

}
//删除交易记录
void MainWindow::deleteTransaction() {
    QModelIndexList selectedIndexes = ui->transactionTableView->selectionModel()->selectedRows();
    qDebug() << "Selected rows count:" << selectedIndexes.count();  // 调试输出选中行数

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No transaction selected."));
        return;
    }
    if (QMessageBox::question(this, tr("Delete Transaction"), tr("Are you sure you want to delete the selected transaction(s)?")) == QMessageBox::Yes) {
        bool allDeleted = true;

        for (const QModelIndex &index : selectedIndexes) {
            QString transactionId = transactionModel->data(transactionModel->index(index.row(), 0)).toString();
            if (!API::deleteTransaction(transactionId)) {
                allDeleted = false;
                QMessageBox::warning(this, tr("Error"), tr("Failed to delete transaction with ID: %1").arg(transactionId));
            }
        }

        if (allDeleted) {
            QMessageBox::information(this, tr("Success"), tr("Selected transaction(s) deleted successfully."));
        }

        loadTransactions(currentAccountId); // 重新加载交易记录
    }
}

void MainWindow::onAccountListViewContextMenuRequested(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    QAction action1("保存编辑", this);
    QAction action2("删除账户", this);

    connect(&action1, &QAction::triggered, this, &MainWindow::editAccount);
    connect(&action2, &QAction::triggered, this, &MainWindow::deleteAccount);

    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);

    contextMenu.exec(ui->accountListView->viewport()->mapToGlobal(pos));
}
//编辑账户
void MainWindow::editAccount() {
    QModelIndex index = ui->accountListView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("Error"), tr("No account selected."));
        return;
    }
    // 获取编辑后的数据
    bool ok;
    QString
    accountId =  ui->accountListView->model()->data(index, Qt::UserRole + 1).toString(), // 提取存储在 UserRole + 1 中的 ID
    name = ui->accountListView->model()->data(index).toString();
    if (!Database::updateAccount(accountId, name)) {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Failed to update account with ID: %1").arg(accountId));
    }
    if (ok) {
        QMessageBox::information(this, tr("Success"), tr("Selected account(s) deleted successfully."));
    }
    loadTransactions(currentAccountId); // 重新加载交易记录
}
//删除账户
void MainWindow::deleteAccount() {
    QModelIndex index = ui->accountListView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("Error"), tr("No Account selected."));
        return;
    }
    if (QMessageBox::question(this, tr("Delete Account"), tr("Are you sure to delete the selected Account(s)?")) == QMessageBox::Yes) {
        bool allDeleted = true;

        QString accountId = ui->accountListView->model()->data(index, Qt::UserRole + 1).toString(); // 提取存储在 UserRole + 1 中的 ID
        if (!API::deleteAccount(accountId)) {
            allDeleted = false;
            QMessageBox::warning(this, tr("Error"), tr("Failed to delete account with ID: %1").arg(accountId));
        }

        if (allDeleted) {
            QMessageBox::information(this, tr("Success"), tr("Selected account(s) deleted successfully."));
        }

        loadAccounts();// 重新加载交易记录
    }
}
