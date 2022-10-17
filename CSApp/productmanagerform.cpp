#include "productmanagerform.h"
#include "ui_productmanagerform.h"
#include "productitem.h"

#include <QFile>
#include <QMenu>
#include <QMessageBox>

ProductManagerForm::ProductManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    //sizes << 200 << 540;
    sizes << 200 << 54000;
    ui->splitter->setSizes(sizes);

    ui->priceLineEdit->setValidator( new QIntValidator(0, 999999999, this) );
    ui->stockLineEdit->setValidator( new QIntValidator(0, 999999999, this) );

    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));

    ui->searchTypeComboBox->hide();    
}

void ProductManagerForm::loadData()
{
    QFile file("productList.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            ProductItem* c = new ProductItem(id, row[1], row[2], row[3].toInt(), row[4].toInt());
            ui->treeWidget->addTopLevelItem(c);
            productList.insert(id, c);
        }
    }
    file.close( );
}

ProductManagerForm::~ProductManagerForm()
{
    delete ui;

    QFile file("productList.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : qAsConst(productList)) {
        ProductItem* c = v;
        out << c->id() << ", " << c->getType() << ", ";
        out << c->getName() << ", " << c->getPrice() << ", ";
        out << c->getStock() << "\n";
    }
    file.close( );
}

int ProductManagerForm::makeId( )
{
    if(productList.size( ) == 0) {
        return 1001;
    } else {
        auto id = productList.lastKey();
        return ++id;
    }
}

void ProductManagerForm::removeItem()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        productList.remove(item->text(0).toInt());
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
//        delete item;
        ui->treeWidget->update();
    }
}

void ProductManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

void ProductManagerForm::cleanInputLineEdit()
{
    ui->idLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->priceLineEdit->clear();
    ui->stockLineEdit->clear();
}

void ProductManagerForm::on_searchPushButton_clicked()
{
    int i = ui->searchComboBox->currentIndex();

    auto flag = (i == 2)? Qt::MatchCaseSensitive|Qt::MatchContains
                        : Qt::MatchCaseSensitive;

    QString str;
    if(i == 1)
        str = ui->searchTypeComboBox->currentText();
    else {
        str = ui->searchLineEdit->text();
        if(str == "") {
            QMessageBox::information(this, tr("Search error"),
                                     tr("Please enter a search term."), QMessageBox::Ok);
            return;
        }
    }

    auto items = ui->treeWidget->findItems(str, flag, i);

    for (const auto& v : qAsConst(productList))
        v->setHidden(true);

    foreach(auto i, items)
        i->setHidden(false);

}

void ProductManagerForm::on_modifyPushButton_clicked()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        ProductItem* p = productList[key];
        QString type, name, price, stock;
        type = ui->typeComboBox->currentText();
        name = ui->nameLineEdit->text();
        price = ui->priceLineEdit->text();
        stock = ui->stockLineEdit->text();

        if(name.length() && price.length() && stock.length()) {
            p->setType(type);
            p->setName(name);
            p->setPrice(price.toInt());
            p->setStock(stock.toInt());
            productList[key] = p;

            //cleanInputLineEdit();
        }
        else {
            QMessageBox::information(this, tr("Modify error"),
               QString(tr("Some items have not been entered.")), QMessageBox::Ok);
        }
    }
}

void ProductManagerForm::on_addPushButton_clicked()
{
    QString type, name, price, stock;
    int id = makeId( );
    type = ui->typeComboBox->currentText();
    name = ui->nameLineEdit->text();
    price = ui->priceLineEdit->text();
    stock = ui->stockLineEdit->text();

    if(name.length() && price.length() && stock.length()) {
        ProductItem* c = new ProductItem(id, type, name, price.toInt(), stock.toInt());
        productList.insert(id, c);
        ui->treeWidget->addTopLevelItem(c);

        cleanInputLineEdit();
    }
    else {
        QMessageBox::information(this, tr("Add error"),
           QString(tr("Some items have not been entered.")), QMessageBox::Ok);
    }
}

void ProductManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));
    ui->typeComboBox->setCurrentText(item->text(1));
    ui->nameLineEdit->setText(item->text(2));
    ui->priceLineEdit->setText(item->text(3));
    ui->stockLineEdit->setText(item->text(4));
}


void ProductManagerForm::on_showAllPushButton_clicked()
{
    for (const auto& v : qAsConst(productList)) {
        v->setHidden(false);
    }
    ui->searchLineEdit->clear();
}

void ProductManagerForm::on_cleanPushButton_clicked()
{
    cleanInputLineEdit();
}


void ProductManagerForm::on_searchComboBox_currentIndexChanged(int index)
{
    if(index == 1) {
        ui->searchLineEdit->hide();
        ui->searchTypeComboBox->show();
    }

    else {
        ui->searchTypeComboBox->hide();
        ui->searchLineEdit->show();
    }
}

void ProductManagerForm::receiveWord(QString word)
{
    QMap<int, ProductItem*> searchList;

    auto flag = Qt::MatchCaseSensitive|Qt::MatchContains;

    auto items1 = ui->treeWidget->findItems(word, flag, 0);
    foreach(auto i, items1) {
        ProductItem* p = static_cast<ProductItem*>(i);
        searchList.insert(p->id(), p);
    }

    auto items2 = ui->treeWidget->findItems(word, flag, 2);
    foreach(auto i, items2) {
        ProductItem* p = static_cast<ProductItem*>(i);
        searchList.insert(p->id(), p);
    }

    for (const auto& v : qAsConst(searchList)) {
        ProductItem* p = v;
        emit sendProductToDialog((ProductItem*)p);
    }
}

void ProductManagerForm::receiveId(int id)
{
    for (const auto& v : qAsConst(productList)) {
        ProductItem* p = v;
        if(p->id() == id) {
            emit sendProductToManager(p);
        }
    }
}


