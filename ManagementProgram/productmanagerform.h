#ifndef PRODUCTMANAGERFORM_H
#define PRODUCTMANAGERFORM_H

#include <QWidget>
#include <QMap>

#include "productitem.h"

class QMenu;

namespace Ui {
class ProductManagerForm;
}

/**
* @brief 제품 정보를 관리하는 클래스
*/
class ProductManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManagerForm(QWidget *parent = nullptr);
    ~ProductManagerForm();

    void loadData(); // 저장되어 있는 제품 리스트 불러오기

private slots:
    void on_showAllPushButton_clicked(); // 전체 제품 리스트 출력 버튼 슬롯
    // 검색 항목 선택 콤보 박스에서 선택된 것이 변경되었을 때의 슬롯
    void on_searchComboBox_currentIndexChanged(int index);
    void on_searchPushButton_clicked();  // 검색 버튼 슬롯
    void on_addPushButton_clicked();     // 제품 추가 버튼 슬롯
    void on_modifyPushButton_clicked();  // 제품 정보 변경 버튼 슬롯
    void on_cleanPushButton_clicked();   // 입력 창 클리어 버튼 슬롯
    // tree widget에서 제품을 클릭(선택)했을 때의 슬롯
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void showContextMenu(const QPoint &); // tree widget의 context 메뉴 출력
    void removeItem();                    // 제품 정보 삭제
    // 주문 정보 관리 객체에서 제품ID를 가지고 제품을 검색 하기 위한 슬롯
    void receiveId(int);
    // 제품 검색 Dialog에서 제품을 검색 하기 위한 슬롯
    void receiveWord(QString);

signals:
    // 제품 검색 Dialog로 검색된 제품 정보를 보내주는 시그널
    void sendProductToDialog(ProductItem*);
    // 주문 정보 관리 객체로 검색된 제품 정보를 보내주는 시그널
    void sendProductToManager(ProductItem*);

private:
    int makeId();              // Id를 자동으로 생성
    void cleanInputLineEdit(); // 입력 창 클리어

    QMap<int, ProductItem*> productList; // 제품 리스트
    Ui::ProductManagerForm *ui;          // ui
    QMenu* menu;                         // tree widget context 메뉴
};

#endif // PRODUCTMANAGERFORM_H
