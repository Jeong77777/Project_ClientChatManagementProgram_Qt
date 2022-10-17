#ifndef ORDERMANAGERFORM_H
#define ORDERMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "orderitem.h"

class QMenu;
class QTreeWidgetItem;
class ClientDialog;
class ProductDialog;
class ClientItem;
class ProductItem;

namespace Ui {
class OrderManagerForm;
}

class OrderManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManagerForm(QWidget *parent = nullptr,
                              ClientDialog *clientDialog = nullptr, ProductDialog *productDialog = nullptr);
    ~OrderManagerForm();

    ClientDialog *clientDialog;
    ProductDialog *productDialog;

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &);
    void removeItem();              /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_searchPushButton_clicked();
    void on_showAllPushButton_clicked();
    void on_cleanPushButton_clicked();


    void on_inputClientPushButton_clicked();
    void on_inputProductPushButton_clicked();

    void receiveClientInfo(ClientItem*);
    void receiveProductInfo(ProductItem*);

    void on_searchComboBox_currentIndexChanged(int index);

signals:
    void sendClientId(int);
    void sendProductId(int);

private:
    int makeId();

    QMap<int, OrderItem*> orderList;
    Ui::OrderManagerForm *ui;
    QMenu* menu;
    ClientItem *searchedClient;
    ProductItem *searchedProduct;
    bool searchedClientFlag;
    bool searchedProductFlag;

    void cleanInputLineEdit();
};

#endif // ORDERMANAGERFORM_H
