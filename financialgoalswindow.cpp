#include "financialgoalswindow.h"
#include "ui_financialgoalswindow.h"
#include "database.h"
#include "mainwindow.h"

#include <QDate>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QPushButton>
#include <QToolBar>
#include <QTimer>
#include <QMessageBox>
#include <QStringListModel>
//using namespace QtCharts;

financialgoalswindow::financialgoalswindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::financialgoalswindow)
{
    ui->setupUi(this);


    QToolBar *returntoolBar = addToolBar("ReturnToolBar");
    addToolBar(Qt::LeftToolBarArea,returntoolBar);

    QPushButton *FinancialReturnButton = new QPushButton(this);
    FinancialReturnButton->setText("返回");
    QPushButton *ingoalsButton = new QPushButton(this);
    ingoalsButton->setText("储蓄计划");
    QPushButton *outgoalsButton = new QPushButton(this);
    outgoalsButton->setText("消费计划");

    returntoolBar->addWidget(FinancialReturnButton);
    returntoolBar->addWidget(ingoalsButton);
    returntoolBar->addWidget(outgoalsButton);

    connect(FinancialReturnButton, &QPushButton::clicked, this, &financialgoalswindow::on_FinancialgoalsReturnButton_clicked);
    connect(ingoalsButton, &QPushButton::clicked, this, &financialgoalswindow::on_ingoalsButton_clicked);
    connect(outgoalsButton, &QPushButton::clicked, this, &financialgoalswindow::on_outgoalsButton_clicked);

    QIcon con1(":/new/button/image/returnbutton.jpg");
    FinancialReturnButton->setIcon(con1);
    QIcon con2(":/new/button/image/ingoalsButton.jpg");
    ingoalsButton->setIcon(con2);
    QIcon con3(":/new/button/image/outgoalsButton.jpg");
    outgoalsButton->setIcon(con3);

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    this->setStyleSheet("QMainWindow {background-image:url(:/new/background/image/background.jpg)}");


}

financialgoalswindow::~financialgoalswindow()
{
    delete ui;
}

void financialgoalswindow::recvMainWindow()
{
    this->show(); // 显示本界面
}

void financialgoalswindow::on_FinancialgoalsReturnButton_clicked() // 按钮槽函数
{
    this->hide();            // 隐藏本界面
    emit showMainWindow();    // 激活信号
}

void financialgoalswindow::recvingoalswindow()
{
    this->show(); // 显示本界面
}

void financialgoalswindow::recvoutgoalswindow()
{
    this->show(); // 显示本界面
}

void financialgoalswindow::on_ingoalsButton_clicked() // 按钮槽函数
{
    this->hide();            // 隐藏本界面
    emit showingoalsWindow();    // 激活信号
}

void financialgoalswindow::on_outgoalsButton_clicked() // 按钮槽函数
{
    this->hide();            // 隐藏本界面
    emit showoutgoalsWindow();    // 激活信号
}


