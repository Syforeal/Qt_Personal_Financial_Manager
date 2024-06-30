#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

bool Database::openDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("finance.db");
    if (!db.open()) {
        qDebug() << "Error: unable to open database" << db.lastError();
        return false;
    }

    // 创建表结构
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS accounts ("
               "id TEXT PRIMARY KEY, "
               "name TEXT, "
               "type TEXT, "  // 自定义账户类型，如微信、支付宝
               "balance REAL)");
    query.exec("CREATE TABLE IF NOT EXISTS transactions ("
               "id TEXT PRIMARY KEY, "
               "account_id TEXT, "
               "amount REAL, "
               "date DATE, "
               "category TEXT, "
               //"description TEXT,"
               "FOREIGN KEY (account_id) REFERENCES accounts(id))");
    query.exec("CREATE TABLE IF NOT EXISTS categories ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT UNIQUE)");
    query.exec("CREATE TABLE IF NOT EXISTS budgets ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "start_date DATE, "
               "amount REAL, "
               "account_id TEXT, "
               "FOREIGN KEY (account_id) REFERENCES accounts(id))");
    query.exec("CREATE TABLE IF NOT EXISTS bills ("
               "id TEXT PRIMARY KEY, "
               "account_id TEXT, "
               "amount REAL, "
               "date DATE, "
               "category TEXT, "
               "FOREIGN KEY (account_id) REFERENCES accounts(id))");
    query.exec("CREATE TABLE IF NOT EXISTS savings ("
               "id TEXT PRIMARY KEY, "
               "account_id TEXT, "
               "amount REAL, "
               "startdate DATE, "
               "enddate DATE, "
               "category TEXT, "
               "FOREIGN KEY (account_id) REFERENCES accounts(id))");

    return true;
}

QList<Account> Database::getAccounts() { //获取所有账户信息
    QList<Account> accounts;
    QSqlQuery query("SELECT * FROM accounts");
    while (query.next()) {
        Account account;
        account.id = query.value("id").toString();
        account.name = query.value("name").toString();
        account.type = query.value("type").toString();
        account.balance = query.value("balance").toDouble();
        accounts.append(account);
    }
    return accounts;
}

Account Database::getAccount(const QString& accountId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM accounts WHERE id = :id");
    query.bindValue(":id", accountId);
    query.exec();
    Account account;
    if (query.next()) {
        account.id = query.value("id").toString();
        account.name = query.value("name").toString();
        account.type = query.value("type").toString();
        account.balance = query.value("balance").toDouble();
    }
    return account;
}

bool Database::addAccount(const Account& account) {
    QSqlQuery query;
    query.prepare("INSERT INTO accounts (id, name, type, balance) VALUES (:id, :name, :type, :balance)");
    query.bindValue(":id", account.id);
    query.bindValue(":name", account.name);
    query.bindValue(":type", account.type);
    query.bindValue(":balance", account.balance);
    return query.exec();
}


double Database::updateAccountBalance(const QString &accountId, const QDate& startDate, const QDate& endDate) {
    double newBalance = getTotalIncome(accountId, startDate, endDate) - getTotalSpent(accountId, startDate, endDate);
    return newBalance;
}


bool Database::updateAccount(const QString& accountId, QString& name) {
    QSqlQuery query;
    query.prepare("UPDATE accounts SET name = :name WHERE id = :id");
    query.bindValue(":name", name);/*
    query.bindValue(":type", account.type);
    query.bindValue(":balance", account.balance);*/
    query.bindValue(":id", accountId);
    return query.exec();
}

bool Database::deleteAccount(const QString& accountId) {
    QSqlQuery query;
    query.prepare("DELETE FROM accounts WHERE id = :id");
    query.bindValue(":id", accountId);
    return query.exec();
}

//根据
QList<Transaction> Database::getTransactions(const QString &accountId) {
    QSqlDatabase db = QSqlDatabase::database();
    QList<Transaction> transactions;

    if (db.isOpen()) {
        QSqlQuery query;
        query.prepare("SELECT id, account_id, amount, date, category FROM transactions WHERE account_id = :account_id ORDER BY date DESC");
        query.bindValue(":account_id", accountId);

        if (query.exec()) {
            while (query.next()) {
                Transaction transaction;
                transaction.id = query.value(0).toString();
                transaction.accountId = query.value(1).toString();
                transaction.amount = query.value(2).toDouble();
                transaction.date = query.value(3).toDate();
                transaction.category = query.value(4).toString();
                transactions.append(transaction);
            }
        } else {
            qDebug() << "Database query error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Database is not open.";
    }

    return transactions;
}
QList<Transaction> Database::getTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount) {
    QSqlDatabase db = QSqlDatabase::database();
    QList<Transaction> transactions;

    if (db.isOpen()) {
        QSqlQuery query;
        QString queryString = "SELECT id, account_id, category, amount, date FROM transactions WHERE account_id = :account_id"; //, description
        if (category != "All Categories") {
            queryString += " AND category = :category";
        }
        queryString += " AND date >= :startDate AND date <= :endDate";
        queryString += " AND amount >= :minAmount AND amount <= :maxAmount";

        query.prepare(queryString);
        query.bindValue(":account_id", accountId);
        if (category != "All Categories") {
            query.bindValue(":category", category);
        }
        query.bindValue(":startDate", startDate);
        query.bindValue(":endDate", endDate);
        query.bindValue(":minAmount", minAmount);
        query.bindValue(":maxAmount", maxAmount);

        if (query.exec()) {
            while (query.next()) {
                Transaction transaction;
                transaction.id = query.value("id").toString();
                transaction.accountId = query.value("account_id").toString();
                transaction.amount = query.value("amount").toDouble();
                transaction.date = query.value("date").toDate();
                transaction.category = query.value("category").toString();
                //transaction.description = query.value("description").toString();
                transactions.append(transaction);
            }
        } else {
            qDebug() << "Database error: " << query.lastError().text();
        }
    } else {
        qDebug() << "Database is not open.";
    }
    return transactions;
}

Transaction Database::getTransaction(const QString &transactionId) {
    QSqlDatabase db = QSqlDatabase::database();
    Transaction transaction;

    if (db.isOpen()) {
        QSqlQuery query;
        query.prepare("SELECT id, accountId, amount, date, category FROM transactions WHERE id = :id");
        query.bindValue(":id", transactionId);

        if (query.exec()) {
            if (query.next()) {
                transaction.id = query.value(0).toString();
                transaction.accountId = query.value(1).toString();
                transaction.amount = query.value(2).toDouble();
                transaction.date = query.value(3).toDate();
                transaction.category = query.value(4).toString();
            } else {
                qDebug() << "No transaction found with id:" << transactionId;
            }
        } else {
            qDebug() << "Database query error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Database is not open.";
    }

    return transaction;
}

bool Database::addTransaction(const Transaction& transaction) {
    QSqlQuery query;
    query.prepare("INSERT INTO transactions (id, account_id, amount, date, category) VALUES (:id, :account_id, :amount, :date, :category)");
    query.bindValue(":id", transaction.id);
    query.bindValue(":account_id", transaction.accountId);
    query.bindValue(":amount", transaction.amount);
    query.bindValue(":date", transaction.date);
    query.bindValue(":category", transaction.category);
    return query.exec();
}

bool Database::updateTransaction(const Transaction &transaction) {
    QSqlQuery query;
    query.prepare("UPDATE transactions SET account_id = :account_id, amount = :amount, date = :date, category = :category WHERE id = :id");//, description = :description
    query.bindValue(":account_id", transaction.accountId);
    query.bindValue(":amount", transaction.amount);
    query.bindValue(":date", transaction.date);
    query.bindValue(":category", transaction.category);
    //query.bindValue(":description", transaction.description);
    query.bindValue(":id", transaction.id);
    if (!query.exec()) {
        qDebug() << "Database update error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteTransaction(const QString& transactionId) {
    QSqlQuery query;
    query.prepare("DELETE FROM transactions WHERE id = :id");
    query.bindValue(":id", transactionId);
    return query.exec();
}

//获取所有分类
QStringList Database::getAllCategories(const QString &accountId) {
    QStringList categories;
    QSqlQuery query;

    query.prepare("SELECT DISTINCT category FROM transactions WHERE account_id = :account_id");
    query.bindValue(":account_id", accountId);
    if (query.exec()) {
        while (query.next()) {
            categories.append(query.value(0).toString());
        }
    } else {
        qDebug() << "Database error: " << query.lastError().text();
    }

    return categories;
}
//过滤/搜索交易记录
QList<Transaction> Database::searchTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount) {
    QList<Transaction> transactions;
    QSqlQuery query;
    QString queryString = "SELECT * FROM transactions WHERE account_id = :account_id";
    if (category != "All Categories") {
        queryString += " AND category = :category";
    }
    queryString += " AND date >= :startDate AND date <= :endDate";
    queryString += " AND amount >= :minAmount AND amount <= :maxAmount";

    query.prepare(queryString);
    query.bindValue(":account_id", accountId);
    if (category != "All Categories") {
        query.bindValue(":category", category);
    }
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);
    query.bindValue(":minAmount", minAmount);
    query.bindValue(":maxAmount", maxAmount);

    if (query.exec()) {
        while (query.next()) {
            Transaction transaction;
            transaction.id = query.value("id").toString();
            transaction.accountId = query.value("accountId").toString();
            transaction.amount = query.value("amount").toDouble();
            transaction.date = query.value("date").toDate();
            transaction.category = query.value("category").toString();
           // transaction.description = query.value("description").toString();
            transactions.append(transaction);
        }
    } else {
        qDebug() << "Database error: " << query.lastError().text();
    }
    return transactions;
}

bool Database::transferFunds(const QString& fromAccountId, const QString& toAccountId, double amount) {
    QSqlQuery query;
    QSqlDatabase::database().transaction(); // 开启事务

    // 减少 fromAccount 的余额
    query.prepare("UPDATE accounts SET balance = balance - :amount WHERE id = :id");
    query.bindValue(":amount", amount);
    query.bindValue(":id", fromAccountId);
    if (!query.exec()) {
        QSqlDatabase::database().rollback();
        return false;
    }

    // 增加 toAccount 的余额
    query.prepare("UPDATE accounts SET balance = balance + :amount WHERE id = :id");
    query.bindValue(":amount", amount);
    query.bindValue(":id", toAccountId);
    if (!query.exec()) {
        QSqlDatabase::database().rollback();
        return false;
    }

    QSqlDatabase::database().commit(); // 提交事务
    return true;
}

bool Database::addTransactions(const QList<Transaction>& transactions) {
    QSqlDatabase::database().transaction(); // 开启事务
    for (const Transaction& transaction : transactions) {
        if (!addTransaction(transaction)) {
            QSqlDatabase::database().rollback();
            return false;
        }
    }
    QSqlDatabase::database().commit(); // 提交事务
    return true;
}

void Database::addCustomCategory(const QString &category) {
    QSqlQuery query;
    query.prepare("INSERT INTO categories (name) VALUES (:name)");
    query.bindValue(":name", category);

    if (!query.exec()) {
        qDebug() << "Error adding category:" << query.lastError().text();
    }
}

QStringList Database::loadCategories() {
    QStringList loadedCategories = {"餐饮", "交通", "购物", "娱乐", "其他"};
    QSqlQuery query("SELECT name FROM categories");

    while (query.next()) {
        QString category = query.value(0).toString();
        loadedCategories.append(category);
    }

    return loadedCategories;
}

QList<Transaction> Database::getAllTransactions() {
    QSqlDatabase db = QSqlDatabase::database();
    QList<Transaction> transactions;

    if (db.isOpen()) {
        QSqlQuery query;
        query.prepare("SELECT id, account_id, amount, date, category FROM transactions");

        if (query.exec()) {
            while (query.next()) {
                Transaction transaction;
                transaction.id = query.value(0).toString();
                transaction.accountId = query.value(1).toString();
                transaction.amount = query.value(2).toDouble();
                transaction.date = query.value(3).toDate();
                transaction.category = query.value(4).toString();
                transactions.append(transaction);
            }
        } else {
            qDebug() << "Database query error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Database is not open.";
    }

    return transactions;
}

//预算
bool Database::addBudget(const QString& accountId, const QDate& startDate, double amount) {
    QSqlQuery query;
    query.prepare("INSERT INTO budgets (start_date, amount, account_id) VALUES (:start_date, :amount, :account_id)");
    query.bindValue(":start_date", startDate);
    query.bindValue(":amount", amount);
    query.bindValue(":account_id", accountId);
    return query.exec();
}

bool Database::updateBudget(int budgetId, double amount) {
    QSqlQuery query;
    query.prepare("UPDATE budgets SET amount = :amount WHERE id = :id");
    query.bindValue(":amount", amount);
    query.bindValue(":id", budgetId);
    return query.exec();
}

bool Database::deleteBudget(int budgetId) {
    QSqlQuery query;
    query.prepare("DELETE FROM budgets WHERE id = :id");
    query.bindValue(":id", budgetId);
    return query.exec();
}

double Database::getBudget(const QString& accountId, const QDate& date) {
    QSqlQuery query;
    query.prepare("SELECT amount FROM budgets WHERE account_id = :account_id AND start_date <= :date ORDER BY start_date DESC LIMIT 1");
    query.bindValue(":account_id", accountId);
    query.bindValue(":date", date);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

double Database::getTotalSpent(const QString& accountId, const QDate& startDate, const QDate& endDate) {
    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM transactions WHERE account_id = :account_id AND date BETWEEN :start_date AND :end_date AND amount < 0");
    query.bindValue(":account_id", accountId);
    query.bindValue(":start_date", startDate);
    query.bindValue(":end_date", endDate);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

double Database::getTotalIncome(const QString& accountId, const QDate& startDate, const QDate& endDate) {
    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM transactions WHERE account_id = :account_id AND date BETWEEN :start_date AND :end_date AND amount > 0");
    query.bindValue(":account_id", accountId);
    query.bindValue(":start_date", startDate);
    query.bindValue(":end_date", endDate);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

QList<bill> Database::getBills(const QString &accountId) {
    QSqlDatabase db = QSqlDatabase::database();
    QList<bill> bills;

    if (db.isOpen()) {
        QSqlQuery query;
        query.prepare("SELECT id, account_id, amount, date, category FROM bills WHERE account_id = :account_id ORDER BY date DESC");
        query.bindValue(":account_id", accountId);

        if (query.exec()) {
            while (query.next()) {
                bill bill;
                bill.id = query.value(0).toString();
                bill.accountId = query.value(1).toString();
                bill.amount = query.value(2).toDouble();
                bill.date = query.value(3).toDate();
                bill.category = query.value(4).toString();
                bills.append(bill);
            }
        } else {
            qDebug() << "Database query error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Database is not open.";
    }

    return bills;
}

bool Database::addBill(const bill& bill) {
    QSqlQuery query;
    query.prepare("INSERT INTO bills (id, account_id, amount, date, category) VALUES (:id, :account_id, :amount, :date, :category)");
    query.bindValue(":id", bill.id);
    query.bindValue(":account_id", bill.accountId);
    query.bindValue(":amount", bill.amount);
    query.bindValue(":date", bill.date);
    query.bindValue(":category", bill.category);
    return query.exec();
}

bool Database::deleteBill(const QString& billId) {
    QSqlQuery query;
    query.prepare("DELETE FROM bills WHERE id = :id");
    query.bindValue(":id", billId);
    return query.exec();
}

QList<saving> Database::getsavings(const QString &accountId) {
    QSqlDatabase db = QSqlDatabase::database();
    QList<saving> savings;

    if (db.isOpen()) {
        QSqlQuery query;
        query.prepare("SELECT id, account_id, amount, startdate, enddate, category FROM savings WHERE account_id = :account_id ORDER BY enddate DESC");
        query.bindValue(":account_id", accountId);

        if (query.exec()) {
            while (query.next()) {
                saving saving;
                saving.id = query.value(0).toString();
                saving.accountId = query.value(1).toString();
                saving.amount = query.value(2).toDouble();
                saving.startdate= query.value(3).toDate();
                saving.enddate = query.value(4).toDate();
                saving.category = query.value(5).toString();
                savings.append(saving);
            }
        } else {
            qDebug() << "Database query error:" << query.lastError().text();
        }
    } else {
        qDebug() << "Database is not open.";
    }

    return savings;
}

bool Database::addsaving(const saving& saving) {
    QSqlQuery query;
    query.prepare("INSERT INTO savings (id, account_id, amount, startdate, enddate, category) VALUES (:id, :account_id, :amount, :startdate, :enddate, :category)");
    query.bindValue(":id", saving.id);
    query.bindValue(":account_id", saving.accountId);
    query.bindValue(":amount", saving.amount);
    query.bindValue(":startdate", saving.startdate);
    query.bindValue(":enddate", saving.enddate);
    query.bindValue(":category", saving.category);
    return query.exec();
}

bool Database::deleteSaving(const QString& savingId) {
    QSqlQuery query;
    query.prepare("DELETE FROM savings WHERE id = :id");
    query.bindValue(":id", savingId);
    return query.exec();
}
