#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanagerform.h"
#include "productmanagerform.h"
#include "ordermanagerform.h"
#include "clientdialog.h"
#include "productdialog.h"
#include "chatserverform.h"

#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Dialog
    ClientDialog *clientDialog = new ClientDialog();
    ProductDialog *productDialog = new ProductDialog();

    // client manager form
    clientForm = new ClientManagerForm(this);
    connect(clientForm, SIGNAL(destroyed()),
            clientForm, SLOT(deleteLater()));
    clientForm->setWindowTitle(tr("Client Info"));

    // product manager form
    productForm = new ProductManagerForm(this);
    connect(productForm, SIGNAL(destroyed()),
            productForm, SLOT(deleteLater()));
    productForm->setWindowTitle(tr("Product Info"));

    // order manager form
    orderForm = new OrderManagerForm(this, clientDialog, productDialog);
    connect(orderForm, SIGNAL(destroyed()),
            orderForm, SLOT(deleteLater()));
    orderForm->setWindowTitle(tr("Order Info"));

    // chat manager form
    chatForm = new ChatServerForm(this);
    connect(chatForm, SIGNAL(destroyed()),
            chatForm, SLOT(deleteLater()));
    chatForm->setWindowTitle(tr("Chat server"));


    connect(clientDialog, SIGNAL(sendWord(QString)), clientForm, SLOT(receiveWord(QString)));
    connect(clientForm, SIGNAL(sendClientToDialog(ClientItem*)), clientDialog, SLOT(receiveClientInfo(ClientItem*)));
    connect(productDialog, SIGNAL(sendWord(QString)), productForm, SLOT(receiveWord(QString)));
    connect(productForm, SIGNAL(sendProductToDialog(ProductItem*)), productDialog, SLOT(receiveProductInfo(ProductItem*)));

    connect(orderForm, SIGNAL(sendClientId(int)), clientForm, SLOT(receiveId(int)));
    connect(clientForm, SIGNAL(sendClientToOrderManager(ClientItem*)), orderForm, SLOT(receiveClientInfo(ClientItem*)));
    connect(orderForm, SIGNAL(sendProductId(int)), productForm, SLOT(receiveId(int)));
    connect(productForm, SIGNAL(sendProductToManager(ProductItem*)), orderForm, SLOT(receiveProductInfo(ProductItem*)));

    connect(clientForm, SIGNAL(sendClientToChatServer(int, QString)), chatForm, SLOT(addClient(int,QString)));

    // mdi
    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
    ui->mdiArea->addSubWindow(orderForm);
    ui->mdiArea->addSubWindow(chatForm);
    ui->mdiArea->setActiveSubWindow(cw);

    clientForm->loadData();
    productForm->loadData();
    orderForm->loadData();

}

MainWindow::~MainWindow()
{
    delete ui;
}

