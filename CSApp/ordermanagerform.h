#ifndef ORDERMANAGERFORM_H
#define ORDERMANAGERFORM_H

#include <QWidget>

namespace Ui {
class OrderManagerForm;
}

class OrderManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManagerForm(QWidget *parent = nullptr);
    ~OrderManagerForm();

private:
    Ui::OrderManagerForm *ui;
};

#endif // ORDERMANAGERFORM_H
