#include "chatlogitem.h"


ChatLogItem::ChatLogItem(QString senderIpPort, QString senderIpName, QString message,\
                         QString receiverIpPort, QString receiverIdName, QString time)
{
    setText(0, senderIpPort);
    setText(1, senderIpName);
    setText(2, message);
    setText(3, receiverIpPort);
    setText(4, receiverIdName);
    setText(5, time);
}

QString ChatLogItem::getSenderIpPort() const
{
    return text(0);
}
QString ChatLogItem::getSenderIpName() const
{
    return text(1);
}
QString ChatLogItem::getMessage() const
{
    return text(2);
}
QString ChatLogItem::getReceiverIpPort() const
{
    return text(3);
}
QString ChatLogItem::getReceiverIdName() const
{
    return text(4);
}
QString ChatLogItem::getTime() const
{
    return text(5);
}
