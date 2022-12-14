#ifndef ORDERMANAGERFORM_H
#define ORDERMANAGERFORM_H

#include <QWidget>
#include <QMap>

#include "orderitem.h"

class QMenu;
class ClientDialog;
class ProductDialog;
class ClientItem;
class ProductItem;

namespace Ui {
class OrderManagerForm;
}

/**
* @brief 주문 정보를 관리하는 클래스
*/
class OrderManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManagerForm(QWidget *parent = nullptr,
                              ClientDialog *clientDialog = nullptr, ProductDialog *productDialog = nullptr);
    ~OrderManagerForm();

    void loadData();  // 저장되어 있는 주문 리스트 불러오기

private slots:
    void on_showAllPushButton_clicked(); // 전체 주문 리스트 출력 버튼 슬롯
    // 검색 항목 선택 콤보 박스에서 선택된 것이 변경되었을 때의 슬롯
    void on_searchComboBox_currentIndexChanged(int index);
    void on_searchPushButton_clicked();       // 검색 버튼 슬롯
    void on_inputClientPushButton_clicked();  // 고객 검색, 입력 버튼 슬롯
    void on_inputProductPushButton_clicked(); // 제품 검색, 입력 버튼 슬롯
    void on_addPushButton_clicked();          // 주문 추가 버튼 슬롯
    void on_modifyPushButton_clicked();       // 주문 정보 변경 버튼 슬롯
    void on_cleanPushButton_clicked();        // 입력 창 클리어 버튼 슬롯
    // tree widget에서 주문을 클릭(선택)했을 때의 슬롯
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void showContextMenu(const QPoint &); // tree widget의 context 메뉴 출력
    void removeItem();                    // 주문 정보 삭제
    // 고객 정보 관리 객체로부터 고객 정보를 받기 위한 슬롯
    void receiveClientInfo(ClientItem*);
    // 제품 정보 관리 객체로부터 제품 정보를 받기 위한 슬롯
    void receiveProductInfo(ProductItem*);

signals:
    // 고객 정보 관리 객체로 가져올 고객 정보에 대한 ID를 보내는 시그널
    void sendClientId(int);
    // 제품 정보 관리 객체로 가져올 제품 정보에 대한 ID를 보내는 시그널
    void sendProductId(int);

private:
    int makeId();              // Id를 자동으로 생성
    void cleanInputLineEdit(); // 입력 창 클리어

    ClientDialog *clientDialog;   // 고객을 검색, 입력하기 위한 다이얼로그
    ProductDialog *productDialog; // 제품을 검색, 입력하기 위한 다이얼로그

    QMap<int, OrderItem*> orderList; // 주문 리스트
    Ui::OrderManagerForm *ui;        // ui
    QMenu* menu;                     // tree widget context 메뉴

    ClientItem *searchedClient;   // 고객 정보 관리 객체로부터 가져온 고객
    ProductItem *searchedProduct; // 제품 정보 관리 객체로부터 가져온 제품
    bool searchedClientFlag;      // 고객 검색 결과 (결과가 있으면 true, 없으면 false)
    bool searchedProductFlag;     // 제품 검색 결과 (결과가 있으면 true, 없으면 false)
};

#endif // ORDERMANAGERFORM_H
