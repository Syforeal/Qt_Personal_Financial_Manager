#include "financialgoals.h"
#include "ui_financialgoals.h"

financialgoals::financialgoals(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::financialgoals)
{
    ui->setupUi(this);
}

financialgoals::~financialgoals()
{
    delete ui;
}
