#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

class Account
{
public:
    QString id;
    QString name;
    QString type;  // 自定义账户类型，如微信、支付宝
    double balance;
};

#endif // ACCOUNT_H
