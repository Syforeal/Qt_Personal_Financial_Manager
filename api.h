#ifndef API_H
#define API_H

#include <QList>
#include "account.h"
#include "transaction.h"

class API {
public:
    static QList<Account> getAccounts();
    static Account getAccount(const QString& accountId);
    static bool addAccount(const Account& account);
    static bool updateAccount(const Account& account);
    static bool deleteAccount(const QString& accountId);

    static QList<Transaction> getTransactions(const QString& accountId);
    static Transaction getTransaction(const QString& transactionId);//新增，未使用
    static bool addTransaction(const Transaction& transaction);
    static bool transferFunds(const QString& fromAccountId, const QString& toAccountId, double amount);
    static bool importTransactionsFromFile(const QString& filePath, const QString& accountId);  // 从文件导入交易记录
};

#endif // API_H
