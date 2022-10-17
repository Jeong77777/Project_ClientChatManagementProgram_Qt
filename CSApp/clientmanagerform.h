#ifndef CLIENTMANAGERFORM_H
#define CLIENTMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "clientitem.h"

class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ClientManagerForm;
}

class ClientManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManagerForm(QWidget *parent = nullptr);
    ~ClientManagerForm();
    void loadData();

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
    void receiveWord(QString);
    void receiveId(int);
    void receiveFromMain();

signals:
    void sendClientToDialog(ClientItem*);
    void sendClientToOrderManager(ClientItem*);
    void sendClientToChatManager(ClientItem*);

private:
    int makeId();

    QMap<int, ClientItem*> clientList;
    Ui::ClientManagerForm *ui;
    QMenu* menu;

    void cleanInputLineEdit();
};

#endif // CLIENTMANAGERFORM_H
