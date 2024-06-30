#ifndef API_H
#define API_H

#include <QList>
#include "account.h"
#include "transaction.h"
#include "bill.h"
#include "saving.h"

class API {
public:
    static QList<Account> getAccounts();
    static Account getAccount(const QString& accountId);
    static bool addAccount(const Account& account);
    static bool updateAccount(const QString& accountId, QString& name);
    static bool deleteAccount(const QString& accountId);

    static QList<Transaction> getTransactions(const QString& accountId);
    static Transaction getTransaction(const QString& transactionId);//新增，未使用
    static bool addTransaction(const Transaction& transaction);
    //static bool updateTransaction(const QString& transactionId, double amount,  const QString &category, const QDate &date /*, description*/);
    static bool updateTransaction(const Transaction& transaction);
    static bool transferFunds(const QString& fromAccountId, const QString& toAccountId, double amount);
    static bool importTransactionsFromFile(const QString& filePath, const QString& accountId);  // 从文件导入交易记录
    static bool deleteTransaction(const QString& transactionId);

    static QList<bill> getBills(const QString& accountId);
    static bool addBill(const bill& bill);
    static bool deleteBill(const QString& billId);

    static QList<saving> getsavings(const QString& accountId);
    static bool addsaving(const saving& saving);
    static bool deleteSaving(const QString& savingId);
    //过滤功能
    static QStringList getAllCategories(const QString &accountId);
    static QList<Transaction> getTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount);
    static QList<Transaction> searchTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount);
    //图表
    static QList<Transaction> getAllTransactions();

};

#endif // API_H
