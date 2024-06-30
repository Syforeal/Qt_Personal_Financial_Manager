#ifndef OUTGOALS_H
#define OUTGOALS_H

#include <QMainWindow>

namespace Ui {
class outgoals;
}

class outgoals : public QMainWindow
{
    Q_OBJECT

public:
    explicit outgoals(QWidget *parent = nullptr);
    ~outgoals();

private:
    Ui::outgoals *ui;
};

#endif // OUTGOALS_H
