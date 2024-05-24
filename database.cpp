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

bool Database::updateAccount(const Account& account) {
    QSqlQuery query;
    query.prepare("UPDATE accounts SET name = :name, type = :type, balance = :balance WHERE id = :id");
    query.bindValue(":name", account.name);
    query.bindValue(":type", account.type);
    query.bindValue(":balance", account.balance);
    query.bindValue(":id", account.id);
    return query.exec();
}

bool Database::deleteAccount(const QString& accountId) {
    QSqlQuery query;
    query.prepare("DELETE FROM accounts WHERE id = :id");
    query.bindValue(":id", accountId);
    return query.exec();
}

QList<Transaction> Database::getTransactions(const QString &accountId) {
    QSqlDatabase db = QSqlDatabase::database();
    QList<Transaction> transactions;

    if (db.isOpen()) {
        QSqlQuery query;
        query.prepare("SELECT id, account_id, amount, date, category FROM transactions WHERE account_id = :account_id");
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
