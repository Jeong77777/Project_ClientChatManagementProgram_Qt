#include "productmanagerform.h"
#include "ui_productmanagerform.h"

ProductManagerForm::ProductManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManagerForm)
{
    ui->setupUi(this);
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, "100");
    item->setText(1, "정원기");
    item->setText(2, "010-1234-4567");
    item->setText(3, "대한민국");
}

ProductManagerForm::~ProductManagerForm()
{
    delete ui;
}
