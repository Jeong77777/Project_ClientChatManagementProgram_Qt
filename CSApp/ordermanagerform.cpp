#include "ordermanagerform.h"
#include "ui_ordermanagerform.h"
#include "orderitem.h"

#include <QFile>
#include <QMenu>

OrderManagerForm::OrderManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 200 << 540;
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
    ui->dateEdit->setDate(QDate(2000,01,01));
    ui->nameLineEdit->clear();
    ui->productLineEdit->clear();
    ui->quantityLineEdit->clear();
}

void OrderManagerForm::on_searchPushButton_clicked()
{
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
//    QTreeWidgetItem* item = ui->treeWidget->currentItem();
//    if(item != nullptr) {
//        int key = item->text(0).toInt();
//        ClientItem* c = clientList[key];
//        QString name, number, address;
//        name = ui->nameLineEdit->text();
//        number = ui->phoneNumberLineEdit->text();
//        address = ui->addressLineEdit->text();

//        if(name.length() && number.length() && address.length()) {
//            c->setName(name);
//            c->setPhoneNumber(number);
//            c->setAddress(address);
//            clientList[key] = c;

//            //cleanInputLineEdit();
//        }
//        else {
//            QMessageBox::information(this, tr("Add error"),
//               QString(tr("Some items have not been entered.")), QMessageBox::Ok);
//        }
//    }
}

void OrderManagerForm::on_addPushButton_clicked()
{
//    QString name, number, address;
//    int id = makeId( );
//    name = ui->nameLineEdit->text();
//    number = ui->phoneNumberLineEdit->text();
//    address = ui->addressLineEdit->text();

//    if(name.length() && number.length() && address.length()) {
//        ClientItem* c = new ClientItem(id, name, number, address);
//        clientList.insert(id, c);
//        ui->treeWidget->addTopLevelItem(c);

//        cleanInputLineEdit();
//    }
//    else {
//        QMessageBox::information(this, tr("Add error"),
//           QString(tr("Some items have not been entered.")), QMessageBox::Ok);
//    }
}

void OrderManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
//    Q_UNUSED(column);
//    ui->idLineEdit->setText(item->text(0));
//    ui->nameLineEdit->setText(item->text(1));
//    ui->phoneNumberLineEdit->setText(item->text(2));
//    ui->addressLineEdit->setText(item->text(3));
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

}

