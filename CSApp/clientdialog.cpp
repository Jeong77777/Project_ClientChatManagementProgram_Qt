#include "clientdialog.h"
#include "ui_clientdialog.h"
#include "clientitem.h"

ClientDialog::ClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Client Info"));
    setWindowModality(Qt::ApplicationModal);

    connect(ui->lineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_pushButton_clicked()));
}

ClientDialog::~ClientDialog()
{
    delete ui;
}

void ClientDialog::on_pushButton_clicked()
{
    ui->treeWidget->clear();
    emit sendWord(ui->lineEdit->text());
}

void ClientDialog::receiveClientInfo(ClientItem * c)
{
    ClientItem* client = new ClientItem(c->id(), c->getName(), c->getPhoneNumber(), c->getAddress());
    qDebug() <<" receive in dialog";
    ui->treeWidget->addTopLevelItem(client);
}

QTreeWidgetItem* ClientDialog::getCurrentItem()
{
    return ui->treeWidget->currentItem();
}

void ClientDialog::clearDialog()
{
    ui->treeWidget->clear();
    ui->lineEdit->clear();
}



