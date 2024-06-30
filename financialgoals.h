#ifndef FINANCIALGOALS_H
#define FINANCIALGOALS_H

#include <QMainWindow>

namespace Ui {
class financialgoals;
}

class financialgoals : public QMainWindow
{
    Q_OBJECT

public:
    explicit financialgoals(QWidget *parent = nullptr);
    ~financialgoals();

private:
    Ui::financialgoals *ui;
};

#endif // FINANCIALGOALS_H
