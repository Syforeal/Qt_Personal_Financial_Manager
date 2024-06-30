#include "outgoals.h"
#include "ui_outgoals.h"

outgoals::outgoals(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::outgoals)
{
    ui->setupUi(this);
}

outgoals::~outgoals()
{
    delete ui;
}
