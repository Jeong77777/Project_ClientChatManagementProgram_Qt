#include "clientdialog.h"
#include "ui_clientdialog.h"
#include "clientitem.h"

/**
* @brief 생성자, dialog 초기화
*/
ClientDialog::ClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Client Info"));
    setWindowModality(Qt::ApplicationModal);

    connect(ui->lineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));

    ui->searchPushButton->setFocus();
}

/**
* @brief 소멸자
*/
ClientDialog::~ClientDialog()
{
    delete ui;
}

/**
* @brief 고객 관리 객체로부터 검색 결과를 받는 슬롯
* @param c 검색된 고객
*/
void ClientDialog::receiveClientInfo(ClientItem * c)
{
    /* 검색 결과를 tree widget에 추가 */
    ClientItem* client = new ClientItem(c->id(), c->getName(), \
                                        c->getPhoneNumber(), c->getAddress());
    ui->treeWidget->addTopLevelItem(client);
}

/**
* @brief 현재 선택된 고객 item을 반환
* @return 현재 선택된 고객 item
*/
QTreeWidgetItem* ClientDialog::getCurrentItem()
{
    return ui->treeWidget->currentItem();
}

/**
* @brief 검색 결과, 입력 창 초기화
*/
void ClientDialog::clearDialog()
{
    ui->treeWidget->clear();
    ui->lineEdit->clear();
    ui->searchPushButton->setFocus();
}

/**
* @brief 검색 버튼에 대한 슬롯, 검색 실행
*/
void ClientDialog::on_searchPushButton_clicked()
{
    /* 검색을 위해 고객 관리 객체로 검색어를 전달하는 시그널 emit */
    ui->treeWidget->clear();
    emit sendWord(ui->lineEdit->text());
}

