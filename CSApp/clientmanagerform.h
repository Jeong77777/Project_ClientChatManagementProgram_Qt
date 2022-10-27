#ifndef CLIENTMANAGERFORM_H
#define CLIENTMANAGERFORM_H

#include <QWidget>
#include <QMap>

#include "clientitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ClientManagerForm;
}

/**
* @brief 고객 정보를 관리하는 클래스
*/
class ClientManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManagerForm(QWidget *parent = nullptr);
    ~ClientManagerForm();

    void loadData(); // 저장되어 있는 고객 리스트 불러오기

private slots:
    void on_showAllPushButton_clicked(); // 전체 고객 리스트 출력 버튼 슬롯
    void on_searchPushButton_clicked();  // 검색 버튼 슬롯
    void on_addPushButton_clicked();     // 고객 추가 버튼 슬롯
    void on_modifyPushButton_clicked();  // 고객 정보 변경 버튼 슬롯
    void on_cleanPushButton_clicked();   // 입력 창 클리어 버튼 슬롯
    // 트리 위젯의 아이템 클릭
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void showContextMenu(const QPoint &); // context 메뉴 출력
    void removeItem();                    // 고객 정보 제거
    void receiveId(int);                  // 고객ID를 이용한 검색
    void receiveWord(QString);            // 고객ID 또는 이름을 이용한 검색

signals:
    void sendClientToDialog(ClientItem*);       // 고객 검색 Dialog로 고객 정보를 보내주는 시그널
    void sendClientToOrderManager(ClientItem*); // 주문 내역 관리 객체로 검색된 고객 정보를 보내주는 시그널
    void sendClientToChatServer(int, QString);  // 채팅 서버로 고객 정보를 보내주는 시그널

private:
    int makeId();              // Id를 자동으로 생성
    void cleanInputLineEdit(); // 입력 창 클리어

    QMap<int, ClientItem*> clientList; // 고객 리스트
    Ui::ClientManagerForm *ui;         // ui
    QMenu* menu;                       // context 메뉴
};

#endif // CLIENTMANAGERFORM_H
