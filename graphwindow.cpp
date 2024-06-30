#include "graphwindow.h"
#include "ui_graphwindow.h"
#include "database.h"
#include "api.h"

#include <QPushButton>
#include <QToolBar>
#include <QDateTime>
#include <QMessageBox>
#include <QToolTip>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

GraphWindow::GraphWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphWindow)
    , chartView(new QChartView(this))  // 初始化 QChartView
    , chartView_2(new QChartView(this))
{
    ui->setupUi(this);

    Database::openDatabase();

    QToolBar *returntoolBar = addToolBar("ReturnToolBar");
    addToolBar(Qt::LeftToolBarArea,returntoolBar);

    QPushButton *GraphReturnButton = new QPushButton(this);
    GraphReturnButton->setText("返回");

    returntoolBar->addWidget(GraphReturnButton);

    connect(GraphReturnButton,&QPushButton::clicked,this,&GraphWindow::on_GraphReturnButton_clicked);

    QIcon con(":/new/button/image/returnbutton.jpg");
    GraphReturnButton->setIcon(con);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");

    // 图表；将 chartView 添加到 chartWidget
    chartLayout = new QVBoxLayout(ui->chartWidget);
    chartLayout->addWidget(chartView);
    connect(ui->generateReportButton, &QPushButton::clicked, this, &GraphWindow::generateYearlyIncomeExpenseTrend);
    //connect(ui->generateWeeklyReportButton, &QPushButton::clicked, this, &GraphWindow::generateWeeklyIncomeExpenseTrend);
    // 添加选择月份的控件
    monthComboBox = new QComboBox(this);
    returntoolBar->addWidget(monthComboBox);
    connect(monthComboBox, &QComboBox::currentTextChanged, this, &GraphWindow::onMonthSelected);

    // 添加选择周的控件
    weekComboBox = new QComboBox(this);
    returntoolBar->addWidget(weekComboBox);
    connect(weekComboBox, &QComboBox::currentTextChanged, this, &GraphWindow::onWeekSelected);

    populateComboBoxes(); // 填充选择框

    chartLayout_2 = new QVBoxLayout(ui->chartWidget_2);
    chartLayout_2->addWidget(chartView_2);
    connect(ui->CategoryExpenseReportButton, &QPushButton::clicked, this, &GraphWindow::generateCategoryExpense);
    connect(ui->CategoryIncomeReportButton, &QPushButton::clicked, this, &GraphWindow::generateCategoryIncome);

    // 允许鼠标交互
    chartView->setMouseTracking(true);
    chartView_2->setMouseTracking(true);
    QToolTip::setFont(QFont("SansSerif", 10));
    QToolTip::setPalette(QPalette(Qt::white, Qt::black));

    QIcon con1(":/new/button/image/tendbutton.jpeg");
    ui->generateReportButton->setIcon(con1);

    QIcon con2(":/new/button/image/inbutton.jpeg");
    ui->CategoryIncomeReportButton->setIcon(con2);

    QIcon con3(":/new/button/image/outbutton.jpeg");
    ui->CategoryExpenseReportButton->setIcon(con3);

}

GraphWindow::~GraphWindow()
{
    delete ui;
    delete yearlyIncomeExpenseChart;
    delete dailyIncomeExpenseChart;
    delete dailyIncomeExpenseChartForWeek;
    delete categoryExpenseDistributionChart;
    delete chartView;
    delete chartView_2;
}

void GraphWindow::recvMainWindow()
{
    this->show(); //显示本界面
}

void GraphWindow::on_GraphReturnButton_clicked() //按钮槽函数
{
    this->hide();            //隐藏本界面
    emit showMainWindow();    //激活信号
}


void GraphWindow::populateComboBoxes() {
    // 获取所有交易记录
    QList<Transaction> transactions = API::getAllTransactions();

    QSet<QString> months;
    QSet<QString> weeks;

    for (const Transaction &transaction : transactions) {
        QString month = transaction.date.toString("yyyy-MM");
        QString week = getWeek(transaction.date);
        qDebug() << "week: " << week;
        months.insert(month);
        weeks.insert(week);
    }

    monthComboBox->addItems(months.values());
    weekComboBox->addItems(weeks.values());
}

void GraphWindow::onMonthSelected(const QString& month) {
    if (!month.isEmpty()) {
        generateDailyIncomeExpenseTrend(month);
    }
}

void GraphWindow::onWeekSelected(const QString& week) {
    if (!week.isEmpty()) {
        generateDailyIncomeExpenseTrendForWeek(week);
    }
}

//生成报表
void GraphWindow::generateIncomeExpenseTrend(QChart*& chart, const QString& title, const QString& dateFormat, const QString& logPrefix, const QString& filter = QString()) {
    qDebug() << logPrefix << "Starting generateIncomeExpenseTrend()with filter: " << filter;

    // 初始化或更新图表
    if (!chart) {
        chart = new QChart();
        chart->setTitle(title);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        qDebug() << logPrefix << "Created new chart";
    } else {
        // 清除旧的系列数据
        chart->removeAllSeries();
        QList<QAbstractAxis *> axes = chart->axes();
        for (QAbstractAxis *axis : axes) {
            chart->removeAxis(axis);
            delete axis;
        }
        qDebug() << logPrefix << "Cleared old series from chart";
        chart->setTitle(title); // 更新图表标题
    }

    // 获取数据
    QList<Transaction> transactions = API::getAllTransactions();
    qDebug() << logPrefix << "Fetched transactions:" << transactions.size();

    QMap<QString, double> incomeData;
    QMap<QString, double> expenseData;

    for (const Transaction &transaction : transactions) {
        QString date = transaction.date.toString(dateFormat);
        // 检查日期是否匹配过滤器（如果提供）
        //transaction.date.toString("yyyy-ww") == filter
        if (filter.isEmpty() || (filter.length() == 7 && getWeek(transaction.date) == filter) || transaction.date.toString("yyyy-MM") == filter) {
            if (transaction.amount > 0) {
                incomeData[date] += transaction.amount;
                qDebug() << logPrefix << "IncomeData[" << date << "]:" << incomeData[date];
            } else {
                expenseData[date] += qAbs(transaction.amount);
                qDebug() << logPrefix << "ExpenseData[" << date << "]:" << expenseData[date];
            }
        }
    }

    qDebug() << logPrefix << "Processed income and expense data";
    qDebug() << logPrefix << "Income data:" << incomeData;
    qDebug() << logPrefix << "Expense data:" << expenseData;

    QBarSet *incomeSet = new QBarSet("Income");
    QBarSet *expenseSet = new QBarSet("Expense");

    QStringList categories;
    for (const QString &date : incomeData.keys()) {
        categories << date;
        *incomeSet << incomeData[date];
        *expenseSet << (expenseData.contains(date) ? expenseData[date] : 0.0);
    }

    for (const QString &date : expenseData.keys()) {
        if (!incomeData.contains(date)) {
            categories << date;
            *incomeSet << 0.0;
            *expenseSet << expenseData[date];
        }
    }

    QBarSeries *series = new QBarSeries();
    series->append(incomeSet);
    series->append(expenseSet);
    chart->addSeries(series);

    //图表交互
    //connect(series, &QBarSeries::hovered, this, &GraphWindow::onBarHovered);
    connect(incomeSet, &QBarSet::hovered, this, &GraphWindow::onBarHovered);
    connect(expenseSet, &QBarSet::hovered, this, &GraphWindow::onBarHovered);
    connect(incomeSet, &QBarSet::clicked, this, &GraphWindow::onBarClicked);
    connect(expenseSet, &QBarSet::clicked, this, &GraphWindow::onBarClicked);

    qDebug() << logPrefix << "Added series to chart";

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    double maxIncome = 0;
    double maxExpense = 0;
    for (double value : std::as_const(incomeData)) {
        if (value > maxIncome) {
            maxIncome = value;
        }
    }
    for (double value : std::as_const(expenseData)) {
        if (value > maxExpense) {
            maxExpense = value;
        }
    }
    axisY->setRange(0, std::max(maxIncome, maxExpense));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    qDebug() << logPrefix << "Configured axes for chart";

    // 确保 chartView 存在并显示图表
    if (!chartView) {
        chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartLayout->addWidget(chartView);
        qDebug() << logPrefix << "Created and added chartView to layout";
    } else {
        // 隐藏当前显示的图表
        if (chartView->chart() == categoryExpenseDistributionChart) {
            categoryExpenseDistributionChart->setVisible(false);
            qDebug() << logPrefix << "Hid categoryExpenseDistributionChart";
        }
        chartView->setChart(chart);
        chart->setVisible(true);
    }

    qDebug() << logPrefix << "Finished generateIncomeExpenseTrend()";
}

void GraphWindow::generateYearlyIncomeExpenseTrend() {
    generateIncomeExpenseTrend(yearlyIncomeExpenseChart, "年度收支趋势", "yyyy-MM", "[Yearly]");
}
/*
void GraphWindow::generateMonthlyIncomeExpenseTrend() {
    generateIncomeExpenseTrend(monthlyIncomeExpenseChart, "月度收支趋势", "yyyy-MM-dd", "[Monthly]");
}

void GraphWindow::generateWeeklyIncomeExpenseTrend() {
    generateIncomeExpenseTrend(weeklyIncomeExpenseChart, "周度收支趋势", "yyyy-MM-dd", "[Weekly]");
}*/

void GraphWindow::generateDailyIncomeExpenseTrend(const QString& month) {
    generateIncomeExpenseTrend(dailyIncomeExpenseChart, month + "月 每日收支趋势", "yyyy-MM-dd", "[Daily]", month);
}

void GraphWindow::generateDailyIncomeExpenseTrendForWeek(const QString& week) {
    generateIncomeExpenseTrend(dailyIncomeExpenseChartForWeek, week + "周 每日收支趋势", "yyyy-MM-dd", "[Daily]", week);
}


void GraphWindow::generateCategoryExpenseDistribution(QChart*& chart, const QString& title, bool expense) {
    qDebug() << "Starting generateCategoryExpenseDistribution()";
    // 初始化或更新各类别支出分布图表
    if (!chart) {
        chart = new QChart();
        chart->setTitle(title);
        chart->setAnimationOptions(QChart::SeriesAnimations);
    } else {
        // 清除旧的系列数据
        chart->removeAllSeries();
        chart->setTitle(title);
    }

    // 获取所有交易记录
    QList<Transaction> transactions = API::getAllTransactions();
    qDebug() << "Fetched transactions:" << transactions.size();

    // 准备数据
    QMap<QString, double> categoryData;
    for (const Transaction &transaction : transactions) {
        if(expense){
            if (transaction.amount < 0) { //支出
                categoryData[transaction.category] += qAbs(transaction.amount);
            }
        }else{
            if (transaction.amount > 0) { //收入
                categoryData[transaction.category] += transaction.amount;
            }
        }
    }

    // 创建饼图数据集
    QPieSeries *series = new QPieSeries();
    for (auto it = categoryData.begin(); it != categoryData.end(); ++it) {
        series->append(it.key(), it.value());
    }

    chart->addSeries(series);

    //图表交互
    for (auto slice : series->slices()) {
        connect(slice, &QPieSlice::hovered, this, &GraphWindow::onPieSliceHovered);
    }

    // 确保 chartView 存在并显示各类别支出分布图表
    if (!chartView_2) {
        chartView_2 = new QChartView(chart);
        chartView_2->setRenderHint(QPainter::Antialiasing);
        chartLayout_2->addWidget(chartView_2);
    } else {
        // 隐藏当前显示的图表
        //if (chartView_2->chart() == monthlyIncomeExpenseChart) {
        //    monthlyIncomeExpenseChart->setVisible(false);
        //}
        chartView_2->setChart(chart);
        chart->setVisible(true);
    }
}

QString GraphWindow::getWeek(const QDate& value)
{
    int year = value.year();
    int week = value.weekNumber();
    QString weekString = QString::number(week).rightJustified(2, '0');
    return QString::number(year) + "-" + weekString;
}

void GraphWindow::generateCategoryExpense() {
    generateCategoryExpenseDistribution(categoryExpenseDistributionChart, "类别支出分布", true);
}
void GraphWindow::generateCategoryIncome() {
    generateCategoryExpenseDistribution(categoryExpenseDistributionChart, "类别收入分布", false);
}
/*
void GraphWindow::onBarHovered(bool status, int index) {
    qDebug() << "onBarHovered called with status:" << status << "and index:" << index;
    QBarSet *barset = qobject_cast<QBarSet *>(sender());
    //if (status) {
        Point globalPos = chartView->mapToGlobal(QCursor::pos());
        QToolTip::showText(globalPos, QString::number(barset->at(index)), chartView);
        qDebug() << "Showing tooltip at:" << globalPos << "with value:" << barset->at(index);
        QToolTip::hideText(); // 强制刷新
        QToolTip::showText(globalPos, QString::number(barset->at(index)), chartView);
        QToolTip::showText(QCursor::pos(), QString::number(barset->at(index)));
    //}else {
    //    QToolTip::hideText();
    //}
    if (barset) { // 检查 barset 是否为 nullptr
        if (index >= 0 && index < barset->count()) { // 检查 index 是否在有效范围内
            if (status) {
                QToolTip::showText(QCursor::pos(), QString::number(barset->at(index)));
            } else {
                QToolTip::hideText();
            }
        } else {
            qDebug() << "Index out of range: " << index;
        }
    } else {
        qDebug() << "Sender is not a QBarSet";
        // 显示错误消息框
        QMessageBox::warning(this, "Error", "Internal error: hovered signal sender is not a QBarSet");
    }
}
void GraphWindow::onBarHovered(bool status, int index, QBarSet *barset) {
    if (status) {
        QString valueStr = QString::number(barset->at(index));
        QToolTip::showText(QCursor::pos(), valueStr);
    } else {
        QToolTip::hideText();
    }
}*/

void GraphWindow::onBarHovered(bool status) {
    QBarSet *barset = qobject_cast<QBarSet *>(sender());
    if (barset) {
        // 获取当前鼠标指针的位置
        QPoint globalPos = QCursor::pos();
        int index = -1;
        // 计算悬停的柱子的索引
        for (int i = 0; i < barset->count(); ++i) {
            if (barset->at(i) == barset->sum()) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            if (status) {
                QToolTip::showText(globalPos, QString::number(barset->at(index)));
            } else {
                QToolTip::hideText();
            }
        }
    } else {
        qDebug() << "Sender is not a QBarSet";
    }
}

void GraphWindow::onBarClicked(int index) {
    QBarSet *barset = qobject_cast<QBarSet *>(sender());
    if (barset) {

        QString date = QString::number(barset->at(index));
        QList<Transaction> transactions = API::getAllTransactions();

        QString details;
        for (const Transaction &transaction : transactions) {
            if (transaction.date.toString("yyyy-MM-dd") == date) {
                details += QString("Category: %1, Amount: %2\n").arg(transaction.category).arg(transaction.amount);
            }
        }
        QMessageBox::information(this, "Details", QString("Clicked value: %1").arg(barset->at(index)));
    }
}/*
void GraphWindow::onBarClicked(int index) {
    QBarSet *barset = qobject_cast<QBarSet *>(sender());
    qDebug() << "Bar clicked: index=" << index << ", barset=" << barset;
    if (barset) {
        QString value = QString::number(barset->at(index));
        qDebug() << "Clicked value:" << value;
        QList<Transaction> transactions = API::getAllTransactions();

        QString details;
        for (const Transaction &transaction : transactions) {
            qDebug() << "Transaction date:" << transaction.date.toString("yyyy-MM-dd") << ", clicked value:" << value;

            if (QString::number(transaction.date.toJulianDay()) == value) { // 使用日期的 Julian Day 进行比较
                details += QString("Category: %1, Amount: %2\n").arg(transaction.category).arg(transaction.amount);
            }
        }
        qDebug() << "Details:" << details;
        if (!details.isEmpty()) {
            QMessageBox::information(this, "Details", details);
        } else {
            QMessageBox::information(this, "Details", "No matching transactions found.");
        }
    }
}

void GraphWindow::onBarClicked(int index, QBarSet *barset) {
    QString date = QString::number(barset->at(index));
    QList<Transaction> transactions = API::getAllTransactions();

    QString details;
    for (const Transaction &transaction : transactions) {
        if (transaction.date.toString("yyyy-MM-dd") == date) {
            details += QString("Category: %1, Amount: %2\n").arg(transaction.category).arg(transaction.amount);
        }
    }

    QMessageBox::information(this, "Transaction Details", details);
}*/

void GraphWindow::onPieSliceHovered(bool status) {
    QPieSlice *slice = qobject_cast<QPieSlice *>(sender());
    /*if (status) {
        double value = slice->value();
        double percentage = slice->percentage() * 100.0;
        QString tooltipText = QString("Value: %1\nPercentage: %2%").arg(value).arg(percentage);
        QToolTip::showText(QCursor::pos(), tooltipText, this, rect(), 3000);
    } else {
        QToolTip::hideText();
    }
    */
    if (status) {
        slice->setLabelVisible(true);
        slice->setLabel(QString("%1: %2 (%3%)")
                            .arg(slice->label())
                            .arg(slice->value())
                            .arg(slice->percentage() * 100, 0, 'f', 1));
    } else {
        slice->setLabelVisible(false);
        slice->setLabel(slice->label().split(":").first());
    }
}
