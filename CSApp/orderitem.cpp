#include "orderitem.h"

#include <QLabel>
//#include <iostream>

using namespace std;

OrderItem::OrderItem(int id, int clientId, QString clientName,
                     int productId, QString productName, int quantity, int total)
{
    this->clientId = clientId;
    this->productId = productId;

    setText(0, QString::number(id));
    setText(1, clientName);
    setText(2, productName);
    setText(3, QString::number(quantity));
    setText(4, QString::number(total));

}

int OrderItem::getClinetId() const
{
    return clientId;
}

void OrderItem::setClientId(int clientId)
{
    this->clientId = clientId;
}

QString OrderItem::getClientName() const
{
    return text(1);
}

void OrderItem::setClientName(QString& clientName)
{
    setText(2, clientName);
}

int OrderItem::getProductId() const
{
    return productId;
}

void OrderItem::setProductId(int productId)
{
    this->productId = productId;
}

QString OrderItem::getProductName() const
{
    return text(2);
}

void OrderItem::setProductName(QString& productName)
{
    setText(2, productName);
}

int OrderItem::getQuantity() const
{
    return text(3).toInt();
}

void OrderItem::setQuantity(int quantity)
{
    setText(3, QString::number(quantity));
}

int OrderItem::id() const
{
    return text(0).toInt();
}
