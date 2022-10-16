#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanagerform.h"
#include "productmanagerform.h"
#include "ordermanagerform.h"
#include "clientdialog.h"
#include "productdialog.h"
#include "chatmanagerform.h"

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
    ProductManagerForm *productForm = new ProductManagerForm(this);
    productForm->setWindowTitle(tr("Product Info"));

    // order manager form
    OrderManagerForm *orderForm = new OrderManagerForm(this, clientDialog, productDialog);
    orderForm->setWindowTitle(tr("Order Info"));

    // chat manager form
    ChatManagerForm *chatForm = new ChatManagerForm(this);
    chatForm->setWindowTitle(tr("Chat Info"));


    connect(clientDialog, SIGNAL(sendWord(QString)), clientForm, SLOT(receiveWord(QString)));
    connect(clientForm, SIGNAL(sendClientToDialog(ClientItem*)), clientDialog, SLOT(receiveClientInfo(ClientItem*)));
    connect(productDialog, SIGNAL(sendWord(QString)), productForm, SLOT(receiveWord(QString)));
    connect(productForm, SIGNAL(sendProductToDialog(ProductItem*)), productDialog, SLOT(receiveProductInfo(ProductItem*)));

    connect(orderForm, SIGNAL(sendClientId(int)), clientForm, SLOT(receiveId(int)));
    connect(clientForm, SIGNAL(sendClientToManager(ClientItem*)), orderForm, SLOT(receiveClientInfo(ClientItem*)));
    connect(orderForm, SIGNAL(sendProductId(int)), productForm, SLOT(receiveId(int)));
    connect(productForm, SIGNAL(sendProductToManager(ProductItem*)), orderForm, SLOT(receiveProductInfo(ProductItem*)));

    // mdi
    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
    ui->mdiArea->addSubWindow(orderForm);
    ui->mdiArea->addSubWindow(chatForm);
    ui->mdiArea->setActiveSubWindow(cw);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionClient_triggered()
{
    if(clientForm != nullptr) {
        clientForm->setFocus();
    }
}

