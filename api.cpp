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

bool API::updateTransaction(const Transaction& transaction) {
    return Database::updateTransaction(transaction);
}

bool API::deleteTransaction(const QString& transactionId){
    return Database::deleteTransaction(transactionId);
}

QList<bill> API::getBills(const QString& accountId){
    return Database::getBills(accountId);
}

bool API::addBill(const bill& bill) {
    return Database::addBill(bill);
}

bool API::deleteBill(const QString& billId){
    return Database::deleteBill(billId);
}

QList<saving> API::getsavings(const QString& accountId){
    return Database::getsavings(accountId);
}

bool API::addsaving(const saving& saving) {
    return Database::addsaving(saving);
}

bool API::deleteSaving(const QString& savingId){
    return Database::deleteSaving(savingId);
}
//新增过滤功能
QStringList API::getAllCategories(const QString &accountId) {
    return Database::getAllCategories(accountId);
}

QList<Transaction> API::getTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount){
    return Database::getTransactions(accountId, category, startDate, endDate, minAmount, maxAmount);
}

QList<Transaction> API::searchTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount){
    return Database::searchTransactions(accountId, category, startDate, endDate, minAmount, maxAmount);
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

//图表
//获取所有交易记录
QList<Transaction> API::getAllTransactions(){
    qDebug() << "Fetching all transactions...";
    return Database::getAllTransactions();
}
