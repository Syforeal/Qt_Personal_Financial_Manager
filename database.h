#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>
#include "account.h"
#include "transaction.h"
#include "bill.h"
#include "saving.h"

class Database {
public:
    static bool openDatabase();
    static QList<Account> getAccounts(); //获取所有账户信息
    static Account getAccount(const QString& accountId);
    static bool addAccount(const Account& account);
    static bool updateAccount(const QString& accountId, QString& name);
    static bool deleteAccount(const QString& accountId);
    static double updateAccountBalance(const QString &accountId, const QDate& startDate, const QDate& endDate);

    static QList<Transaction> getTransactions(const QString &accountId);
    static Transaction getTransaction(const QString &transactionId);//新增，未使用
    static bool addTransaction(const Transaction& transaction);
    static bool updateTransaction(const Transaction &transaction);
    static bool transferFunds(const QString& fromAccountId, const QString& toAccountId, double amount);
    static bool addTransactions(const QList<Transaction>& transactions);  // 批量添加交易记录
    static bool deleteTransaction(const QString& transactionId);

    static QStringList getAllCategories(const QString &accountId);
    static QList<Transaction> getTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount);
    static QList<Transaction> searchTransactions(const QString &accountId, const QString &category, const QDate &startDate, const QDate &endDate, double minAmount, double maxAmount);

    static void addCustomCategory(const QString &category);
    static QStringList loadCategories();

    static QList<Transaction> getAllTransactions();

    //预算
    static bool addBudget(const QString& accountId, const QDate& startDate, double amount);
    static bool updateBudget(int budgetId, double amount);
    static bool deleteBudget(int budgetId);
    static double getBudget(const QString& accountId, const QDate& date);
    static double getTotalSpent(const QString& accountId, const QDate& startDate, const QDate& endDate);
    static double getTotalIncome(const QString& accountId, const QDate& startDate, const QDate& endDate);

    static QList<bill> getBills(const QString &accountId);
    static bool addBill(const bill& bill);
    static bool deleteBill(const QString& billId);

    static QList<saving> getsavings(const QString &accountId);
    static bool addsaving(const saving& saving);
    static bool deleteSaving(const QString& savingId);

};

#endif // DATABASE_H
