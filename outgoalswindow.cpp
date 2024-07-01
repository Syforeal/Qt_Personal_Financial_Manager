#include "outgoalswindow.h"
#include "ui_outgoalswindow.h"
#include "database.h"
#include "api.h"


#include <QPushButton>
#include <QToolBar>
#include <QDate>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QToolBar>
#include <QTimer>
#include <QMessageBox>
#include <QStringListModel>
#include <QStandardItemModel>>
outgoalsWindow::outgoalsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::outgoalsWindow)
{
    ui->setupUi(this);
    Database::openDatabase();
    loadAccounts();

    QToolBar *toolBar = addToolBar("ReturnToolBar");
    addToolBar(Qt::LeftToolBarArea,toolBar);

    QPushButton *ReturnButton = new QPushButton(this);
    ReturnButton->setText("返回");

    QIcon con(":/new/button/image/returnbutton.jpg");
    ReturnButton->setIcon(con);
    QIcon con1(":/new/button/image/savebutton.png");
    ui->saveBudgetButton->setIcon(con1);

    toolBar->addWidget(ReturnButton);

    connect(ReturnButton, &QPushButton::clicked, this, &outgoalsWindow::on_ReturnButton_clicked);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");

    connect(ui->saveBudgetButton, &QPushButton::clicked, this, &outgoalsWindow::onSaveBudgetButtonClicked);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &outgoalsWindow::checkBudgetReminders);
    timer->start(86400000); // 每天检查一次

    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data(Qt::UserRole + 1).toString();
        updateBudgetComparison();
    });
    updateBudgetComparison();
    ui -> amountbox ->setRange(0, 1000000);

    //建立图表
    QChart *chart = new QChart();
    QPieSeries *series = new QPieSeries();
    series->append("已支出", 40);
    series->append("剩余预算", 60);

    QPieSlice *slice = series->slices().at(1);
    slice->setExploded();
    slice->setLabelVisible();
    slice->setPen(QPen(Qt::darkGreen, 2));
    slice->setBrush(Qt::green);

    chart->addSeries(series);
    chart->setTitle("预算视图");

    ui->budgetChartView->setChart(chart);
}

outgoalsWindow::~outgoalsWindow()
{
    delete ui;
}

void outgoalsWindow::recvfinancialgoalsWindow()
{
    loadAccounts();
    connect(ui->accountListView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=](const QModelIndex &current) {
        currentAccountId = current.data(Qt::UserRole + 1).toString();
        updateBudgetComparison();
    });
    this->show(); // 显示本界面
}

void outgoalsWindow::on_ReturnButton_clicked() // 按钮槽函数
{
    this->hide();            // 隐藏本界面
    emit showfinancialgoalswindow();    // 激活信号
}

void outgoalsWindow::loadAccounts() {
    QList<Account> accounts = API::getAccounts();
    QStandardItemModel *model = new QStandardItemModel(accounts.size(), 1, this); // 1 列：用于存储和显示名称
    // 填充模型
    for (int row = 0; row < accounts.size(); ++row) {
        const Account &account = accounts[row];
        QStandardItem *item = new QStandardItem(account.name);
        item->setData(account.id, Qt::UserRole + 1); // 使用 UserRole + 1 存储 ID
        model->setItem(row, 0, item); // 将 item 设置到模型的相应位置
    }

    // 设置模型到 QListView
    ui->accountListView->setModel(model);
}

void outgoalsWindow::onSaveBudgetButtonClicked() {
    QDate startDate = startOfMonth(QDate::currentDate()); //ui->budgetStartDateEdit->date(); //开始日期
    double amount = ui->amountbox->value();

    Database::addBudget(currentAccountId, startDate, amount);

    updateBudgetComparison();
}

void outgoalsWindow::updateBudgetComparison() {
    QString accountId = currentAccountId;
    QDate currentDate = QDate::currentDate();
    double budget = Database::getBudget(accountId, currentDate); //预算
    double spent = Database::getTotalSpent(accountId, startOfMonth(currentDate), endOfMonth(currentDate));
    double remaining = budget + spent;
    double percentRemaining = remaining > 0?(remaining / budget) * 100: 0;

    ui->budgetComparisonLabel->setText(tr("本月预算: %1\n本月支出: %2\n剩余预算: %3\n剩余百分比: %4%")
                                           .arg(budget).arg(spent).arg(remaining).arg(percentRemaining, 0, 'f', 2));

    QPieSeries *series = new QPieSeries();
    series->append("已花费", spent);
    series->append("剩余", remaining);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("预算对比");

    ui->budgetChartView->setChart(chart);
    //图表交互
    for (auto slice : series->slices()) {
        connect(slice, &QPieSlice::hovered, this, &outgoalsWindow::onPieSliceHovered);
    }
}

void outgoalsWindow::onPieSliceHovered(bool status) {
    QPieSlice *slice = qobject_cast<QPieSlice *>(sender());

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

void outgoalsWindow::checkBudgetReminders() {
    QString accountId = currentAccountId;
    QDate currentDate = QDate::currentDate();
    double budget = Database::getBudget(accountId, currentDate);
    double spent = Database::getTotalSpent(accountId, startOfMonth(currentDate), endOfMonth(currentDate));
    double remaining = budget - spent;

    if (remaining <= budget * 0.5) {
        QMessageBox::warning(this, tr("预算提醒"), tr("您的预算剩余不足50%"));
    } else if (remaining <= budget * 0.2) {
        QMessageBox::warning(this, tr("预算提醒"), tr("您的预算剩余不足20%"));
    } else if (remaining <= budget * 0.1) {
        QMessageBox::warning(this, tr("预算提醒"), tr("您的预算剩余不足10%"));
    }

    int daysLeft = currentDate.daysTo(endOfMonth(currentDate));
    if (daysLeft <= 14) {
        QMessageBox::information(this, tr("月末提醒"), tr("距离月末还有14天"));
    } else if (daysLeft <= 7) {
        QMessageBox::information(this, tr("月末提醒"), tr("距离月末还有7天"));
    } else if (daysLeft <= 5) {
        QMessageBox::information(this, tr("月末提醒"), tr("距离月末还有5天"));
    } else if (daysLeft <= 3) {
        QMessageBox::information(this, tr("月末提醒"), tr("距离月末还有3天"));
    }
}
