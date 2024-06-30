#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include "account.h"
#include "transaction.h"
#include "api.h"
#include "database.h"

#include <QMainWindow>
#include <QComboBox>
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QBarSet>
#include <QPieSlice>

namespace Ui {
class GraphWindow;
}

class GraphWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphWindow(QWidget *parent = nullptr);
    ~GraphWindow();

private slots:
    void recvMainWindow();
    void on_GraphReturnButton_clicked();

    void populateComboBoxes();
    void onMonthSelected(const QString& month);
    void onWeekSelected(const QString& week);
    void generateYearlyIncomeExpenseTrend();
    //void generateMonthlyIncomeExpenseTrend();
    //void generateWeeklyIncomeExpenseTrend();
    void generateDailyIncomeExpenseTrend(const QString& month);
    void generateDailyIncomeExpenseTrendForWeek(const QString& week);
    QString getWeek(const QDate& value);

    void generateCategoryExpenseDistribution(QChart*& chart, const QString& title, bool expense);
    void generateCategoryExpense();
    void generateCategoryIncome();

    void onBarHovered(bool status);
    void onBarClicked(int index);
    void onPieSliceHovered(bool status);

private:
    Ui::GraphWindow *ui;

    QChartView *chartView= nullptr;  // 添加 QChartView 变量
    QVBoxLayout *chartLayout = nullptr; // 新增的成员变量
    QChartView *chartView_2= nullptr;
    QVBoxLayout *chartLayout_2 = nullptr;

    QComboBox *monthComboBox = nullptr;
    QComboBox *weekComboBox = nullptr;
    QChart *yearlyIncomeExpenseChart = nullptr;
    QChart *dailyIncomeExpenseChart = nullptr;
    QChart *dailyIncomeExpenseChartForWeek = nullptr;
    QChart *categoryExpenseDistributionChart = nullptr;

    void generateIncomeExpenseTrend(QChart*& chart, const QString& title, const QString& dateFormat, const QString& logPrefix, const QString& filter);


signals:
    void showMainWindow();
};

#endif // GRAPHWINDOW_H
