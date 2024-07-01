#include "mainwindow.h"
#include "database.h"
#include "transactionwindow.h"
#include "financialgoalswindow.h"
#include "billwindow.h"
#include "graphwindow.h"
#include "ingoalswindow.h"
#include "outgoalswindow.h"
#include "api.h"
#include "database.h"
#include <QMessageBox>
#include <QUuid>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStringListModel>
#include <QPushButton>
#include <QToolBar>
#include <QMenu>
#include <QIcon>
#include <QDate>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QtCore>
#include <QObject>
/*
class WeChatBillFetcher : public QObject {
    Q_OBJECT

public:
    WeChatBillFetcher(QObject *parent = nullptr);
    void fetchBill(const QString &billDate, const QString &billType = "ALL", const QString &tarType = "");

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
};

WeChatBillFetcher::WeChatBillFetcher(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &WeChatBillFetcher::onReplyFinished);
}

void WeChatBillFetcher::fetchBill(const QString &billDate, const QString &billType, const QString &tarType) {
    QUrl url("https://api.mch.weixin.qq.com/v3/bill/tradebill");
    QUrlQuery query;
    query.addQueryItem("bill_date", billDate);
    query.addQueryItem("bill_type", billType);
    if (!tarType.isEmpty()) {
        query.addQueryItem("tar_type", tarType);
    }
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 添加认证信息，如商户号、API密钥等
    // request.setRawHeader("Authorization", "Bearer YOUR_ACCESS_TOKEN");

    manager->get(request);
}

void WeChatBillFetcher::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            QString downloadUrl = jsonObj["download_url"].toString();
            QString hashType = jsonObj["hash_type"].toString();
            QString hashValue = jsonObj["hash_value"].toString();

            qDebug() << "Download URL:" << downloadUrl;
            qDebug() << "Hash Type:" << hashType;
            qDebug() << "Hash Value:" << hashValue;

            // 可以在这里下载账单文件并进行处理
        }
    } else {
        qDebug() << "Error:" << reply->errorString();
    }
    reply->deleteLater();
}
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // 打开数据库连接
    if (!Database::openDatabase()) {
        qFatal("Failed to open database.");
        return -1;
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "financia_manager_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    TransactionWindow tw;
    financialgoalswindow fw;
    BillWindow bw;
    GraphWindow gw;
    ingoalsWindow iw;
    outgoalsWindow ow;
    MainWindow w;
    w.show();
    QList<Account> Accounts=API::getAccounts();
    qDebug()<<Accounts.size();
    for(int i=0;i<Accounts.size();i++)
    {
        QList<bill> bills=API::getBills(Accounts[i].name);
        for(int j=0;j<bills.size();j++)
        {
            QDate startDate = QDate::currentDate();
            QDate endDate = bills[j].date;
            if(startDate.daysTo(endDate) == 0)
            {
                QMessageBox msgBox;
                QString str = QString::number(bills[j].amount);
                msgBox.setText(Accounts[i].name +" "+ bills[j].date.toString() +" "+ bills[j].category +" "+ str +"元");  // 设置文本内容
                msgBox.setWindowTitle("账单到期提醒(已到期）");  // 设置标题
                msgBox.setIcon(QMessageBox::Information);  // 设置图标
                msgBox.addButton(QMessageBox::Ok);  // 添加按钮
                msgBox.exec();
            }
            else if(startDate.daysTo(endDate)<=5&&startDate.daysTo(endDate)>0)
            {
                QMessageBox msgBox;
                QString str = QString::number(bills[j].amount);
                msgBox.setText(Accounts[i].name +" "+ bills[j].date.toString() +" "+ bills[j].category +" "+ str +"元" +" 剩余 "+QString::number(startDate.daysTo(endDate))+" 天");  // 设置文本内容
                msgBox.setWindowTitle("账单到期提醒(即将到期）");  // 设置标题
                msgBox.setIcon(QMessageBox::Information);  // 设置图标
                msgBox.addButton(QMessageBox::Ok);  // 添加按钮
                msgBox.exec();
            }
        }
    }

    QObject::connect(&w,SIGNAL(showtransactionwindow()),&tw,SLOT(recvMainWindow()));
    QObject::connect(&tw,SIGNAL(showMainWindow()),&w,SLOT(recvtransactionwindow()));

    QObject::connect(&w,SIGNAL(showfinancialgoalswindow()),&fw,SLOT(recvMainWindow()));
    QObject::connect(&fw,SIGNAL(showMainWindow()),&w,SLOT(recvfinancialgoalswindow()));

    QObject::connect(&w,SIGNAL(showbillwindow()),&bw,SLOT(recvMainWindow()));
    QObject::connect(&bw,SIGNAL(showMainWindow()),&w,SLOT(recvbillwindow()));

    QObject::connect(&w,SIGNAL(showgraphwindow()),&gw,SLOT(recvMainWindow()));
    QObject::connect(&gw,SIGNAL(showMainWindow()),&w,SLOT(recvgraphwindow()));

    QObject::connect(&fw,SIGNAL(showingoalsWindow()),&iw,SLOT(recvfinancialgoalsWindow()));
    QObject::connect(&iw,SIGNAL(showfinancialgoalswindow()),&fw,SLOT(recvingoalswindow()));

    QObject::connect(&fw,SIGNAL(showoutgoalsWindow()),&ow,SLOT(recvfinancialgoalsWindow()));
    QObject::connect(&ow,SIGNAL(showfinancialgoalswindow()),&fw,SLOT(recvoutgoalswindow()));
    return a.exec();
}
