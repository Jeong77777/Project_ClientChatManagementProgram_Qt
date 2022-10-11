#include "ordermanagerform.h"
#include "ui_ordermanagerform.h"

OrderManagerForm::OrderManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManagerForm)
{
    ui->setupUi(this);
}

OrderManagerForm::~OrderManagerForm()
{
    delete ui;
}
