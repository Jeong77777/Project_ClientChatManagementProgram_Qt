#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //this->setWindowTitle("Chat program for client");
    //this->setBaseSize(600,300);
}

Widget::~Widget()
{
    delete ui;
}

