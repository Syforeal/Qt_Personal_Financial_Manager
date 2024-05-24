#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>
#include "account.h"
#include "transaction.h"

class Database {
public:
    static bool openDatabase();
    static QList<Account> getAccounts(); //获取所有账户信息
    static Account getAccount(const QString& accountId);
    static bool addAccount(const Account& account);
    static bool updateAccount(const Account& account);
    static bool deleteAccount(const QString& accountId);

    static QList<Transaction> getTransactions(const QString &accountId);
    static Transaction getTransaction(const QString &transactionId);//新增，未使用
    static bool addTransaction(const Transaction& transaction);
    static bool transferFunds(const QString& fromAccountId, const QString& toAccountId, double amount);
    static bool addTransactions(const QList<Transaction>& transactions);  // 批量添加交易记录
};

#endif // DATABASE_H
