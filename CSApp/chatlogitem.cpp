#include "chatlogitem.h"


ChatLogItem::ChatLogItem(QString ip, QString port,
                           QString id, QString name, QString message, QString time)
{
    setText(0, ip);
    setText(1, port);
    setText(2, id);
    setText(3, name);
    setText(4, message);
    setText(5, time);
}
QString ChatLogItem::getIP() const
{
    return text(0);
}
QString ChatLogItem::getPort() const
{
    return text(1);
}

QString ChatLogItem::getID() const
{
    return text(2);
}

QString ChatLogItem::getName() const
{
    return text(3);
}
QString ChatLogItem::getMessage() const
{
    return text(4);
}
QString ChatLogItem::getTime() const
{
    return text(5);
}
