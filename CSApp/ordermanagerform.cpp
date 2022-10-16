#include "ordermanagerform.h"
#include "ui_clientdialog.h"
#include "ui_ordermanagerform.h"
#include "orderitem.h"
#include "clientdialog.h"
#include "productdialog.h"
#include "clientitem.h"
#include "productitem.h"
#include "ui_productdialog.h"

#include <QFile>
#include <QMenu>
#include <QMessageBox>

OrderManagerForm::OrderManagerForm(QWidget *parent, ClientDialog *clientDialog, ProductDialog *productDialog) :
    QWidget(parent), clientDialog(clientDialog), productDialog(productDialog),
    ui(new Ui::OrderManagerForm)
{
    ui->setupUi(this);

    searchedClientFlag = false;
    searchedProductFlag = false;
    cleanInputLineEdit();

    QList<int> sizes;
    sizes << 200 << 54000;
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));

    QFile file("orderlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            OrderItem* c = new OrderItem(id, row[1], row[2].toInt(), row[3],
                    row[4].toInt(), row[5], row[6].toInt(), row[7]);
            ui->treeWidget->addTopLevelItem(c);
            orderList.insert(id, c);
        }
    }
    file.close( );
}

OrderManagerForm::~OrderManagerForm()
{
    delete ui;

    QFile file("orderlist.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        for (const auto& v : orderList) {
            OrderItem* c = v;
            out << c->id() << ", " << c->getDate() << ", ";
            out << c->getClinetId() << ", " << c->getClientName() << ", ";
            out << c->getProductId() << ", " << c->getProductName() << ", ";
            out << c->getQuantity() << ", " << c->getTotal() << "\n";
        }
        file.close( );
}

int OrderManagerForm::makeId( )
{
    if(orderList.size( ) == 0) {
        return 1000001;
    } else {
        auto id = orderList.lastKey();
        return ++id;
    }
}

void OrderManagerForm::removeItem()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        orderList.remove(item->text(0).toInt());
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
//        delete item;
        ui->treeWidget->update();
    }
}

void OrderManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

void OrderManagerForm::cleanInputLineEdit()
{
    ui->idLineEdit->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->clientLineEdit->clear();
    ui->productLineEdit->clear();
    ui->quantitySpinBox->clear();
}

void OrderManagerForm::on_searchPushButton_clicked()
{
    // 검색기능 고치기
    int i = ui->searchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->treeWidget->findItems(ui->searchLineEdit->text(), flag, i);

        for (const auto& v : qAsConst(orderList))
            v->setHidden(true);

        foreach(auto i, items)
            i->setHidden(false);
    }
}

void OrderManagerForm::on_modifyPushButton_clicked()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        OrderItem* o = orderList[key];

        QString date, clientName, productName, total;
        int clientId, productId, oldQuantity, newQuantity;

        clientId = ui->clientLineEdit->text().split(" ")[0].toInt();
        productId = ui->productLineEdit->text().split(" ")[0].toInt();
        oldQuantity = o->getQuantity();
        newQuantity = ui->quantitySpinBox->text().toInt();
        clientName = ui->clientLineEdit->text();
        productName = ui->productLineEdit->text();
        date = ui->dateEdit->date().toString("yyyy-MM-dd");

        emit sendClientId(clientId);
        emit sendProductId(productId);

        try {
            if(searchedClientFlag == false)
                throw tr("Customer information does not exist.");
            else if(searchedProductFlag == false)
                throw tr("Product information does not exist.");
            else if(newQuantity == 0)
                throw tr("Please enter a valid quantity.");
            else if(searchedProduct->getStock() + oldQuantity < newQuantity)
                throw tr("There is a shortage of stock.\n") + tr("Maximum: ")
                    + QString::number(searchedProduct->getStock() + oldQuantity);

            total = QString::number(newQuantity * searchedProduct->getPrice());
            searchedProduct->setStock(searchedProduct->getStock() + oldQuantity - newQuantity);

            o->setDate(date);
            o->setClientId(clientId);
            o->setClientName(clientName);
            o->setProductId(productId);
            o->setProductName(productName);
            o->setQuantity(newQuantity);
            o->setTotal(total);
            orderList[key] = o;

            on_treeWidget_itemClicked(item, 0);
        }
        catch (QString msg) {
            QMessageBox::information(this, tr("Add error"),
                                     QString(msg), QMessageBox::Ok);
        }

        searchedClientFlag = false;
        searchedProductFlag = false;
    }
}

void OrderManagerForm::on_addPushButton_clicked()
{
    QString date, clientName, productName, total;
    int id = makeId( );
    int clientId, productId, quantity;

    clientId = ui->clientLineEdit->text().split(" ")[0].toInt();
    productId = ui->productLineEdit->text().split(" ")[0].toInt();
    quantity = ui->quantitySpinBox->text().toInt();
    clientName = ui->clientLineEdit->text();
    productName = ui->productLineEdit->text();
    date = ui->dateEdit->date().toString("yyyy-MM-dd");

    qDebug() <<"clientId: "<<clientId;
    qDebug() <<"productId: "<<productId;

    emit sendClientId(clientId);
    emit sendProductId(productId);

    qDebug() <<"searchedClientFlag: "<<searchedClientFlag;
    qDebug() <<"searchedProductFlag: "<<searchedProductFlag;


    try {
        if(searchedClientFlag == false)
            throw tr("Customer information does not exist.");
        else if(searchedProductFlag == false)
            throw tr("Product information does not exist.");
        else if(quantity == 0)
            throw tr("Please enter a valid quantity.");
        else if(searchedProduct->getStock() < quantity)
            throw tr("There is a shortage of stock.");

        total = QString::number(quantity * searchedProduct->getPrice());
        OrderItem* o = new OrderItem(id, date, clientId, clientName,
                                     productId, productName, quantity, total);
        orderList.insert(id, o);
        ui->treeWidget->addTopLevelItem(o);

        searchedProduct->setStock(searchedProduct->getStock() - quantity);

        cleanInputLineEdit();

    } catch (QString msg) {
        QMessageBox::information(this, tr("Add error"),
                                 QString(msg), QMessageBox::Ok);
    }

    searchedClientFlag = false;
    searchedProductFlag = false;
}

void OrderManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));
    ui->dateEdit->setDate(QDate::fromString(item->text(1), "yyyy-MM-dd"));
    ui->clientLineEdit->setText(item->text(2));
    ui->productLineEdit->setText(item->text(3));
    ui->quantitySpinBox->setValue(item->text(4).toInt());

    int clientId = ui->clientLineEdit->text().split(" ")[0].toInt();
    int productId = ui->productLineEdit->text().split(" ")[0].toInt();
    emit sendClientId(clientId);
    emit sendProductId(productId);

    ui->clientTreeWidget->clear();
    ui->productTreeWidget->clear();
    if(searchedClientFlag == true) {
        ClientItem* c = new ClientItem(searchedClient->id(), searchedClient->getName(),
                                       searchedClient->getPhoneNumber(), searchedClient->getAddress());
        ui->clientTreeWidget->addTopLevelItem(c);
    }
    if(searchedProductFlag == true) {
        ProductItem* c = new ProductItem(searchedProduct->id(), searchedProduct->getType(),
                                       searchedProduct->getName(), searchedProduct->getPrice(), searchedProduct->getStock());
        ui->productTreeWidget->addTopLevelItem(c);
    }
    searchedClientFlag = false;
    searchedProductFlag = false;
}


void OrderManagerForm::on_showAllPushButton_clicked()
{
    for (const auto& v : qAsConst(orderList)) {
        v->setHidden(false);
    }
    ui->searchLineEdit->clear();
}

void OrderManagerForm::on_cleanPushButton_clicked()
{
    cleanInputLineEdit();
}

void OrderManagerForm::on_inputClientPushButton_clicked()
{
    clientDialog->show();

    if (clientDialog->exec() == QDialog::Accepted) {
        ClientItem* c = (ClientItem*)clientDialog->ui->treeWidget->currentItem();
        if(c!=nullptr) {
            ui->clientLineEdit->setText(QString::number(c->id()) + " (" + c->getName() + ")");            
        }
    }
    clientDialog->ui->treeWidget->clear();
    clientDialog->ui->lineEdit->clear();
}

void OrderManagerForm::on_inputProductPushButton_clicked()
{
    qDebug()<<"clicked!!";
    productDialog->show();
    if (productDialog->exec() == QDialog::Accepted) {
        ProductItem* p = (ProductItem*)productDialog->ui->treeWidget->currentItem();
        if(p!=nullptr) {
            ui->productLineEdit->setText(QString::number(p->id()) + " (" + p->getName() + ")");            
        }
    }
    productDialog->ui->treeWidget->clear();
    productDialog->ui->lineEdit->clear();
}

void OrderManagerForm::receiveClientInfo(ClientItem* c)
{
    searchedClient = c;
    searchedClientFlag = true;
}

void OrderManagerForm::receiveProductInfo(ProductItem* p)
{
    searchedProduct = p;
    searchedProductFlag = true;
}
