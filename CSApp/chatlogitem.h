#ifndef CHATLOGITEM_H
#define CHATLOGITEM_H

#include <QTreeWidgetItem>

class ChatLogItem : public QTreeWidgetItem
{
public:
    explicit ChatLogItem(QString="", QString="" , QString = "", QString = "", QString = "",QString="");
    QString getIP() const;
    QString getPort() const;
    QString getID() const;
    QString getName() const;
    QString getMessage() const;
    QString getTime() const;

};

#endif // CHATLOGITEM_H
