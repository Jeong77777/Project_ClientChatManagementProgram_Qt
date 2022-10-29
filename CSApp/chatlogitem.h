#ifndef CHATLOGITEM_H
#define CHATLOGITEM_H

#include <QTreeWidgetItem>

/**
* @brief Chat log를 저장하는 tree widget item 클래스
*/
class ChatLogItem : public QTreeWidgetItem
{
public:
    explicit ChatLogItem(QString="", QString="" , QString = "",\
                         QString = "", QString = "",QString="");
    QString getSenderIpPort() const;
    QString getSenderIpName() const;
    QString getMessage() const;
    QString getReceiverIpPort() const;
    QString getReceiverIdName() const;
    QString getTime() const;

};

#endif // CHATLOGITEM_H
