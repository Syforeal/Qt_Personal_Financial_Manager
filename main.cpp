#include "mainwindow.h"
#include "database.h"

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
    MainWindow w;
    w.show();
    return a.exec();
}
