#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanagerform.h"
#include "productmanagerform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clientForm = new ClientManagerForm(this);
    connect(clientForm, SIGNAL(destroyed()),
            clientForm, SLOT(deleteLater()));
    clientForm->setWindowTitle(tr("Client Info"));    

    ProductManagerForm *productForm = new ProductManagerForm(this);
    productForm->setWindowTitle(tr("Product Info"));

    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
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

