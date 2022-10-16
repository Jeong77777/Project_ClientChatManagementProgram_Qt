#include "chatmanagerform.h"
#include "ui_chatmanagerform.h"

ChatManagerForm::ChatManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatManagerForm)
{
    ui->setupUi(this);
}

ChatManagerForm::~ChatManagerForm()
{
    delete ui;
}
