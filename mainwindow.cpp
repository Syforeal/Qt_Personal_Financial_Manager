#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "api.h"
#include "database.h"
#include <QMessageBox>
#include <QUuid>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //新增（连接ui）

    Database::openDatabase();
    loadAccounts();

    connect(ui->addAccountButton, &QPushButton::clicked, this, &MainWindow::addAccount);
    connect(ui->addTransactionButton, &QPushButton::clicked, this, &MainWindow::addTransaction);
    connect(ui->transferFundsButton, &QPushButton::clicked, this, &MainWindow::transferFunds);
    connect(ui->importTransactionsButton, &QPushButton::clicked, this, &MainWindow::importTransactions);  // 导入交易记录

    // Connect account selection change to load transactions
    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data().toString();
        loadTransactions(currentAccountId);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

//新增
void MainWindow::loadAccounts() {
    QList<Account> accounts = API::getAccounts();
    // Populate the account list view
    // Example with QListView and QStringListModel
    QStringList accountNames;
    for (const Account &account : accounts) {
        accountNames << account.name;
    }
    ui->accountListView->setModel(new QStringListModel(accountNames, this));
}

void MainWindow::loadTransactions(const QString& accountId) {
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


void MainWindow::addAccount() {
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
