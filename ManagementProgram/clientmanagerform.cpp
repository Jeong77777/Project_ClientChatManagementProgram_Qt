#include "clientmanagerform.h"
#include "ui_clientmanagerform.h"
#include "clientitem.h"

#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QValidator>

/**
* @brief 생성자, split 사이즈 설정, 정규 표현식 설정, context 메뉴 설정
*/
ClientManagerForm::ClientManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManagerForm)
{
    ui->setupUi(this);

    /* split 사이즈 설정 */
    QList<int> sizes;
    sizes << 170 << 400;
    ui->splitter->setSizes(sizes);

    /* 핸드폰 번호 입력 칸에 대한 정규 표현식 설정 */
    QRegularExpressionValidator* phoneNumberRegExpValidator \
            = new QRegularExpressionValidator(this);
    phoneNumberRegExpValidator\
            ->setRegularExpression(QRegularExpression("^\\d{2,3}-\\d{3,4}-\\d{4}$"));
    ui->phoneNumberLineEdit->setValidator(phoneNumberRegExpValidator);

    /* tree widget의 context 메뉴 설정 */
    // tree widget에서 고객을 삭제하는 action
    QAction* removeAction = new QAction(tr("Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));
    menu = new QMenu; // context 메뉴
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)),\
            this, SLOT(showContextMenu(QPoint)));

    /* 검색 창에서 enter 키를 누르면 검색 버튼이 클릭되도록 connect */
    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));
}

/**
* @brief clientlist.txt 파일을 열어서 저장된 고객 리스트를 가져옴
*/
void ClientManagerForm::loadData()
{
    /* clientlist.txt 파일을 연다. */
    QFile file("clientlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    /* parsing 후 고객 정보를 tree widget에 추가 */
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            ClientItem* c = new ClientItem(id, row[1], row[2], row[3]);
            ui->treeWidget->addTopLevelItem(c);
            clientList.insert(id, c);

            // 채팅 서버로 고객 정보(id, 이름) 보냄
            emit sendClientToChatServer(id, row[1]);
        }
    }
    file.close( );
}

/**
* @brief 소멸자, 고객 리스트를 clientlist.txt에 저장
*/
ClientManagerForm::~ClientManagerForm()
{
    delete ui;

    /* clientlist.txt 파일을 연다. */
    QFile file("clientlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    /* 구분자를 ", "로 해서 고객 정보를 파일에 저장 */
    QTextStream out(&file);
    for (const auto& v : qAsConst(clientList)) {
        ClientItem* c = v;
        out << c->id() << ", " << c->getName() << ", ";
        out << c->getPhoneNumber() << ", ";
        out << c->getAddress() << "\n";
    }
    file.close( );
}

/**
* @brief 전체 고객 리스트 출력 버튼 슬롯, tree widget에 전체 고객 리스트를 출력해 준다.
*/
void ClientManagerForm::on_showAllPushButton_clicked()
{
    for (const auto& v : qAsConst(clientList)) {
        v->setHidden(false);
    }
    ui->searchLineEdit->clear(); // 검색 창 클리어
}

/**
* @brief 검색 버튼 슬롯, tree widget에 검색 결과를 출력해 준다.
*/
void ClientManagerForm::on_searchPushButton_clicked()
{
    /* 검색어 가져오기 */
    QString str = ui->searchLineEdit->text();
    if(!str.length()) { // 검색 창이 비어 있을 때
        QMessageBox::warning(this, tr("Search error"),
                             tr("Please enter a search term."), QMessageBox::Ok);
        return;
    }

    /* 검색 수행 */
    // 0. ID  1. 이름  2. 전화번호  3. 주소
    int i = ui->searchComboBox->currentIndex();

    // 1 2 3: 대소문자 구분, 부분 일치 검색, 0: 대소문자 구분 검색
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;

    // 검색
    auto items = ui->treeWidget->findItems(str, flag, i);

    /* 검색된 결과만 tree widget에 보여 주기 */
    for (const auto& v : qAsConst(clientList))
        v->setHidden(true);
    foreach(auto i, items)
        i->setHidden(false);
}


/**
* @brief 고객 추가 버튼 슬롯, 입력 창에 입력된 정보에 따라 고객을 추가함
*/
void ClientManagerForm::on_addPushButton_clicked()
{
    /* 입력 창에 입력된 정보 가져오기 */
    QString name, number, address;
    int id = makeId(); // 자동으로 ID 생성
    name = ui->nameLineEdit->text();
    number = ui->phoneNumberLineEdit->text();
    address = ui->addressLineEdit->text();

    /* 입력된 정보로 tree widget item을 생성하고 tree widget에 추가 */
    if(name.length() && number.length() && address.length()) {
        ClientItem* c = new ClientItem(id, name, number, address);
        clientList.insert(id, c);           // 고객 리스트에 추가
        ui->treeWidget->addTopLevelItem(c); // tree widget에 추가

        cleanInputLineEdit(); // 입력 창 클리어

        // 채팅 서버로 신규 고객 정보(id, 이름) 보냄
        emit sendClientToChatServer(id, name); // 채팅 서버로
    }
    else { // 비어있는 입력 창이 있을 때
        QMessageBox::warning(this, tr("Add error"),
                             QString(tr("Some items have not been entered.")),\
                             QMessageBox::Ok);
    }
}

/**
* @brief 고객 정보 변경 버튼 슬롯, 입력 창에 입력된 정보에 따라 고객 정보를 변경함
*/
void ClientManagerForm::on_modifyPushButton_clicked()
{
    /* tree widget에서 현재 선택된 item 가져오기 */
    QTreeWidgetItem* item = ui->treeWidget->currentItem();

    /* 입력 창에 입력된 정보에 따라 고객 정보를 변경 */
    if(item != nullptr) {
        // ID를 이용하여 고객 리스트에서 고객 가져오기
        int key = item->text(0).toInt();
        ClientItem* c = clientList[key];

        // 입력 창에 입력된 정보 가져오기
        QString name, number, address;
        name = ui->nameLineEdit->text();
        number = ui->phoneNumberLineEdit->text();
        address = ui->addressLineEdit->text();

        // 입력 창에 입력된 정보에 따라 고객 정보를 변경
        if(name.length() && number.length() && address.length()) {
            c->setName(name);
            c->setPhoneNumber(number);
            c->setAddress(address);
            clientList[key] = c;

            // 채팅 서버로 변경된 고객 정보(id, 이름) 보냄
            emit sendClientToChatServer(c->id(), name);
        }
        else { // 비어있는 입력 창이 있을 때
            QMessageBox::warning(this, tr("Modify error"),\
                                 QString(tr("Some items have not been entered.")),\
                                 QMessageBox::Ok);
        }
    }
}

/**
* @brief 입력 창 클리어 버튼 슬롯, 입력 창을 클리어 하는 함수 호출
*/
void ClientManagerForm::on_cleanPushButton_clicked()
{
    cleanInputLineEdit();
}

/**
* @brief tree widget에서 제품을 클릭(선택)했을 때 실행되는 슬롯, 클릭된 제품의 정보를 입력 창에 표시
* @Param QTreeWidgetItem *item 클릭된 item
* @Param int column 클릭된 item의 열
*/
void ClientManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    /* 클릭된 고객의 정보를 입력 창에 표시해줌 */
    ui->idLineEdit->setText(item->text(0));
    ui->nameLineEdit->setText(item->text(1));
    ui->phoneNumberLineEdit->setText(item->text(2));
    ui->addressLineEdit->setText(item->text(3));
}

/**
* @brief tree widget의 context 메뉴 출력
* @param const QPoint &pos 우클릭한 위치
*/
void ClientManagerForm::showContextMenu(const QPoint &pos)
{
    /* tree widget 위에서 우클릭한 위치에서 context menu 출력 */
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

/**
* @brief 고객 정보 삭제
*/
void ClientManagerForm::removeItem()
{
    /* tree widget에서 현재 선택된 item 가져오기 */
    QTreeWidgetItem* item = ui->treeWidget->currentItem();

    /* 고객 정보 삭제 */
    if(item != nullptr) {
        clientList.remove(item->text(0).toInt()); // 리스트에서 고객 정보 삭제
        // tree widget에서 고객 정보 삭제
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
        delete item;
        ui->treeWidget->update(); // tree widget update
    }
}

/**
* @brief 주문 정보 관리 객체에서 고객ID를 가지고 고객을 검색 하기 위한 슬롯
* @Param int id 검색할 고객 id
*/
void ClientManagerForm::receiveId(int id)
{
    for (const auto& v : qAsConst(clientList)) {
        ClientItem* c = v;
        if(c->id() == id) {
            // 검색 결과를 주문 정보 관리 객체로 보냄
            emit sendClientToOrderManager(c);
        }
    }
}

/**
* @brief 고객 검색 Dialog에서 고객을 검색 하기 위한 슬롯
* @Param QString word 검색어(id 또는 이름)
*/
void ClientManagerForm::receiveWord(QString word)
{
    /* 검색 결과를 저장할 map */
    QMap<int, ClientItem*> searchList;

    /* 대소문자를 구분하고 부분 일치 검색으로 설정 */
    auto flag = Qt::MatchCaseSensitive|Qt::MatchContains;

    /* id에서 검색 */
    auto items1 = ui->treeWidget->findItems(word, flag, 0);
    foreach(auto i, items1) {
        ClientItem* c = static_cast<ClientItem*>(i);
        searchList.insert(c->id(), c); // 검색 결과를 map에 저장
    }

    /* 이름에서 검색 */
    auto items2 = ui->treeWidget->findItems(word, flag, 1);
    foreach(auto i, items2) {
        ClientItem* c = static_cast<ClientItem*>(i);
        searchList.insert(c->id(), c); // 검색 결과를 map에 저장
    }

    /* 검색 결과를 고객 검색 Dialog로 보냄 */
    for (const auto& v : qAsConst(searchList)) {
        ClientItem* c = v;
        emit sendClientToDialog(c);
    }
}

/**
* @brief 신규 고객 추가 시 ID를 자동으로 생성
* @return int 새로운 id 반환
*/
int ClientManagerForm::makeId()
{
    if(clientList.size( ) == 0) {
        return 10001; // id는 10001부터 시작
    } else {
        auto id = clientList.lastKey();
        return ++id; // 기존의 제일 큰 id보다 1만큼 큰 숫자를 반환
    }
}

/**
* @brief 입력 창 클리어
*/
void ClientManagerForm::cleanInputLineEdit()
{
    ui->idLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->phoneNumberLineEdit->clear();
    ui->addressLineEdit->clear();
}
