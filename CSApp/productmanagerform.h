#ifndef PRODUCTMANAGERFORM_H
#define PRODUCTMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "productitem.h"

namespace Ui {
class ProductManagerForm;
}

class ProductManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManagerForm(QWidget *parent = nullptr);
    ~ProductManagerForm();

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

    void on_searchComboBox_currentIndexChanged(int index);

private:
    int makeId();

    QMap<int, ProductItem*> productList;
    Ui::ProductManagerForm *ui;
    QMenu* menu;

    void cleanInputLineEdit();
};

#endif // PRODUCTMANAGERFORM_H
