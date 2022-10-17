#include "productdialog.h"
#include "ui_productdialog.h"
#include "productitem.h"

ProductDialog::ProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProductDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Product Info"));
    setWindowModality(Qt::ApplicationModal);

    connect(ui->lineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_pushButton_clicked()));
}

ProductDialog::~ProductDialog()
{
    delete ui;
}

void ProductDialog::on_pushButton_clicked()
{
    ui->treeWidget->clear();
    emit sendWord(ui->lineEdit->text());
    qDebug() <<"on_pushButton_clicked";
}

void ProductDialog::receiveProductInfo(ProductItem * p)
{
    ProductItem* product = new ProductItem(p->id(), p->getType(), p->getName(), p->getPrice(), p->getStock());
    ui->treeWidget->addTopLevelItem(product);
}

QTreeWidgetItem* ProductDialog::getCurrentItem()
{
    return ui->treeWidget->currentItem();
}

void ProductDialog::clearDialog()
{
    ui->treeWidget->clear();
    ui->lineEdit->clear();
}

