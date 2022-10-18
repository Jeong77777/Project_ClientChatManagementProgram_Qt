#include "clientmanagerform.h"
#include "ui_clientmanagerform.h"
#include "clientitem.h"

#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QValidator>

ClientManagerForm::ClientManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    //sizes << 150 << 550;
    sizes << 170 << 400;
    ui->splitter->setSizes(sizes);

    QRegularExpressionValidator* phoneNumberRegExpValidator = new QRegularExpressionValidator(this);
    phoneNumberRegExpValidator->setRegularExpression(QRegularExpression("^\\d{2,3}-\\d{3,4}-\\d{4}$"));
    ui->phoneNumberLineEdit->setValidator(phoneNumberRegExpValidator);

    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));
}

void ClientManagerForm::loadData()
{
    QFile file("clientlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            ClientItem* c = new ClientItem(id, row[1], row[2], row[3]);
            ui->treeWidget->addTopLevelItem(c);
            clientList.insert(id, c);

            emit sendClientToChatServer(id, row[1]);
        }
    }
    file.close( );
}

ClientManagerForm::~ClientManagerForm()
{
    delete ui;

    QFile file("clientlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : qAsConst(clientList)) {
        ClientItem* c = v;
        out << c->id() << ", " << c->getName() << ", ";
        out << c->getPhoneNumber() << ", ";
        out << c->getAddress() << "\n";
    }
    file.close( );
}

int ClientManagerForm::makeId( )
{
    if(clientList.size( ) == 0) {
        return 10001;
    } else {
        auto id = clientList.lastKey();
        return ++id;
    }
}

void ClientManagerForm::removeItem()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        clientList.remove(item->text(0).toInt());
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
//        delete item;
        ui->treeWidget->update();
    }
}

void ClientManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

void ClientManagerForm::cleanInputLineEdit()
{
    ui->idLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->phoneNumberLineEdit->clear();
    ui->addressLineEdit->clear();
}

void ClientManagerForm::on_searchPushButton_clicked()
{
    int i = ui->searchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;

    QString str = ui->searchLineEdit->text();
    if(str == "") {
        QMessageBox::information(this, tr("Search error"),
                                 tr("Please enter a search term."), QMessageBox::Ok);
        return;
    }

    auto items = ui->treeWidget->findItems(str, flag, i);

    for (const auto& v : qAsConst(clientList))
        v->setHidden(true);

    foreach(auto i, items)
        i->setHidden(false);

}

void ClientManagerForm::on_modifyPushButton_clicked()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        ClientItem* c = clientList[key];
        QString name, number, address;
        name = ui->nameLineEdit->text();
        number = ui->phoneNumberLineEdit->text();
        address = ui->addressLineEdit->text();

        if(name.length() && number.length() && address.length()) {
            c->setName(name);
            c->setPhoneNumber(number);
            c->setAddress(address);
            clientList[key] = c;

            //cleanInputLineEdit();
        }
        else {
            QMessageBox::information(this, tr("Modify error"),
               QString(tr("Some items have not been entered.")), QMessageBox::Ok);
        }
    }
}

void ClientManagerForm::on_addPushButton_clicked()
{
    QString name, number, address;
    int id = makeId( );
    name = ui->nameLineEdit->text();
    number = ui->phoneNumberLineEdit->text();
    address = ui->addressLineEdit->text();

    if(name.length() && number.length() && address.length()) {
        ClientItem* c = new ClientItem(id, name, number, address);
        clientList.insert(id, c);
        ui->treeWidget->addTopLevelItem(c);

        cleanInputLineEdit();

        sendClientToChatServer(id, name);
    }
    else {
        QMessageBox::information(this, tr("Add error"),
           QString(tr("Some items have not been entered.")), QMessageBox::Ok);
    }
}

void ClientManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));
    ui->nameLineEdit->setText(item->text(1));
    ui->phoneNumberLineEdit->setText(item->text(2));
    ui->addressLineEdit->setText(item->text(3));
}


void ClientManagerForm::on_showAllPushButton_clicked()
{
    for (const auto& v : qAsConst(clientList)) {
        v->setHidden(false);
    }
    ui->searchLineEdit->clear();
}

void ClientManagerForm::on_cleanPushButton_clicked()
{
    cleanInputLineEdit();
}

void ClientManagerForm::receiveWord(QString word)
{
    QMap<int, ClientItem*> searchList;

    auto flag = Qt::MatchCaseSensitive|Qt::MatchContains;

    auto items1 = ui->treeWidget->findItems(word, flag, 0);
    foreach(auto i, items1) {
        ClientItem* c = static_cast<ClientItem*>(i);
        searchList.insert(c->id(), c);
    }

    auto items2 = ui->treeWidget->findItems(word, flag, 1);
    foreach(auto i, items2) {
        ClientItem* c = static_cast<ClientItem*>(i);
        searchList.insert(c->id(), c);
    }

    for (const auto& v : qAsConst(searchList)) {
        ClientItem* c = v;
        emit sendClientToDialog((ClientItem*)c);
    }
}

void ClientManagerForm::receiveId(int id)
{
    for (const auto& v : qAsConst(clientList)) {
        ClientItem* c = v;
        if(c->id() == id) {
            emit sendClientToOrderManager(c);
        }
    }
}


