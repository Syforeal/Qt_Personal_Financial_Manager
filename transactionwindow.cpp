#include "transactionwindow.h"
#include "ui_transactionwindow.h"
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
TransactionWindow::TransactionWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TransactionWindow)
{
    ui->setupUi(this);
    //新增（连接ui）
    Database::openDatabase();
    loadAccounts();

    transactionTableView = ui->transactionTableView;

    QToolBar *toolBar = addToolBar("ToolBar");
    addToolBar(Qt::RightToolBarArea,toolBar);
    QPushButton *addAccountButton = new QPushButton(this);
    addAccountButton->setText("添加账户");
    QPushButton *addTransactionButton = new QPushButton(this);
    addTransactionButton->setText("添加交易");
    QPushButton *transferFundsButton = new QPushButton(this);
    transferFundsButton->setText("资金转移");
    QPushButton *importTransactionsButton =new QPushButton(this);
    importTransactionsButton->setText("批量导入交易");

    toolBar->addWidget(addAccountButton);
    toolBar->addWidget(addTransactionButton);
    toolBar->addWidget(transferFundsButton);
    toolBar->addWidget(importTransactionsButton);

    // connect(ui->addTransactionButton, &QPushButton::clicked, this, &MainWindow::addTransaction);
    connect(addAccountButton, &QPushButton::clicked, this, &TransactionWindow::addAccount);
    connect(addTransactionButton, &QPushButton::clicked, this, &TransactionWindow::addTransaction);
    connect(transferFundsButton, &QPushButton::clicked, this, &TransactionWindow::transferFunds);
    connect(importTransactionsButton, &QPushButton::clicked, this, &TransactionWindow::importTransactions);  // 导入交易记录

    // Connect account selection change to load transactions
    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data().toString();
        loadTransactions(currentAccountId);
    });

    QToolBar *returntoolBar = addToolBar("ReturnToolBar");
    addToolBar(Qt::LeftToolBarArea,returntoolBar);

    QPushButton *TransactionReturnButton = new QPushButton(this);
    TransactionReturnButton->setText("返回");

    returntoolBar->addWidget(TransactionReturnButton);

    connect(TransactionReturnButton,&QPushButton::clicked,this,&TransactionWindow::on_TransactionReturnButton_clicked);

    QIcon con1(":/new/button/image/returnbutton.jpg");
    TransactionReturnButton->setIcon(con1);

    QIcon con2(":/new/button/image/addaccountbutton.jpeg");
    addAccountButton->setIcon(con2);

    QIcon con3(":/new/button/image/addtransactionbutton.jpeg");
    addTransactionButton->setIcon(con3);

    QIcon con4(":/new/button/image/transferfundsbutton.jpeg");
    transferFundsButton->setIcon(con4);

    QIcon con5(":/new/button/image/importbutton.jpeg");
    importTransactionsButton->setIcon(con5);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");
}



void TransactionWindow::loadAccounts() {
    QList<Account> accounts = API::getAccounts();
    // Populate the account list view
    // Example with QListView and QStringListModel
    QStringList accountNames;
    for (const Account &account : accounts) {
        accountNames << account.name;
    }
    ui->accountListView->setModel(new QStringListModel(accountNames, this));
}

void TransactionWindow::loadTransactions(const QString& accountId) {
    QList<Transaction> transactions = API::getTransactions(accountId);
    // Populate the transaction table view
    // Example with QTableView and QStandardItemModel
    QStandardItemModel *model = new QStandardItemModel(transactions.size(), 5, this);  // 5 columns: id, accountId, amount, date, category
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("AccountId"));
    model->setHeaderData(2, Qt::Horizontal, tr("Amount"));
    model->setHeaderData(3, Qt::Horizontal, tr("Date"));
    model->setHeaderData(4, Qt::Horizontal, tr("Category"));

    for (int row = 0; row < transactions.size(); ++row) {
        model->setData(model->index(row, 0), transactions[row].id);
        model->setData(model->index(row, 1), transactions[row].accountId);
        model->setData(model->index(row, 2), transactions[row].amount);
        model->setData(model->index(row, 3), transactions[row].date.toString(Qt::ISODate));
        model->setData(model->index(row, 4), transactions[row].category);
    }
    ui->transactionTableView->setModel(model);
}


void TransactionWindow::addAccount() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Account"),
                                         tr("Account Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty()) {
        QStringList accountTypes = {"WeChat", "Alipay"};
        QString type = QInputDialog::getItem(this, tr("Account Type"),
                                             tr("Account Type:"), accountTypes, 0, false, &ok);
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

void TransactionWindow::addTransaction() {
    qDebug() << "Current account ID: " << currentAccountId; // 调试输出
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
        transaction.category = QInputDialog::getText(this, tr("Add Transaction"),
                                                     tr("Category:"), QLineEdit::Normal,
                                                     "", &ok);
        if (ok) {
            if (API::addTransaction(transaction)) {
                QMessageBox::information(this, tr("Success"), tr("Transaction added successfully."));
                loadTransactions(currentAccountId);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to add transaction."));
            }
        }
    }
}

void TransactionWindow::transferFunds() {
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

void TransactionWindow::importTransactions() {
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
void TransactionWindow::recvMainWindow()
{
    this->show(); //显示本界面
}

void TransactionWindow::on_TransactionReturnButton_clicked() //按钮槽函数
{
    this->hide();            //隐藏本界面
    emit showMainWindow();    //激活信号
}
