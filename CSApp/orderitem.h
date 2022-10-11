#ifndef ORDERITEM_H
#define ORDERITEM_H

#include <QTreeWidgetItem>

class OrderItem : public QTreeWidgetItem
{
private:
    int clientId;
    int productId;

public:
    explicit OrderItem(int id = 0, int clientId = 0, QString clientName = "",
                       int productId = 0, QString productName = "", int quantity = 0, int total = 0);

    int getClinetId() const;
    void setClientId(int);
    QString getClientName() const;
    void setClientName(QString&);
    int getProductId() const;
    void setProductId(int);
    QString getProductName() const;
    void setProductName(QString&);
    int getQuantity() const;
    void setQuantity(int);
    int id() const;
    //bool operator==(const OrderItem &other) const;
};

#endif // ORDERITEM_H
