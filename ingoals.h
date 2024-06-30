#ifndef INGOALS_H
#define INGOALS_H

#include <QMainWindow>

namespace Ui {
class ingoals;
}

class ingoals : public QMainWindow
{
    Q_OBJECT

public:
    explicit ingoals(QWidget *parent = nullptr);
    ~ingoals();

private:
    Ui::ingoals *ui;
};

#endif // INGOALS_H
