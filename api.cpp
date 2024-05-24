#include "api.h"
#include "database.h"
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QDebug>
#include <QUuid>

QList<Account> API::getAccounts() {
    return Database::getAccounts();
}

Account API::getAccount(const QString& accountId) {
    return Database::getAccount(accountId);
}

bool API::addAccount(const Account& account) {
    return Database::addAccount(account);
}

bool API::updateAccount(const Account& account) {
    return Database::updateAccount(account);
}

bool API::deleteAccount(const QString& accountId) {
    return Database::deleteAccount(accountId);
}

QList<Transaction> API::getTransactions(const QString& accountId){
    return Database::getTransactions(accountId);
}

Transaction API::getTransaction(const QString& transactionId){//新增，未使用
    return Database::getTransaction(transactionId);
}

bool API::addTransaction(const Transaction& transaction) {
    return Database::addTransaction(transaction);
}

bool API::transferFunds(const QString& fromAccountId, const QString& toAccountId, double amount) {
    return Database::transferFunds(fromAccountId, toAccountId, amount);
}

bool API::importTransactionsFromFile(const QString& filePath, const QString& accountId) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "错误: 文件打开失败" << filePath;
        return false;
    }

    QTextStream in(&file);
    QList<Transaction> transactions;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() < 4) {
            continue;  // 忽略格式错误的行
        }

        Transaction transaction;
        transaction.id = QUuid::createUuid().toString();
        transaction.accountId = accountId;
        transaction.amount = fields[0].toDouble();
        transaction.date = QDate::fromString(fields[1], "yyyy-MM-dd");
        transaction.category = fields[2];
        transactions.append(transaction);
    }

    file.close();
    return Database::addTransactions(transactions);
}

