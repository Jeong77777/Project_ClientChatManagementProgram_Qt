#ifndef CHATLOGITEM_H
#define CHATLOGITEM_H

#include <QTreeWidgetItem>

/**
* @brief 고객 정보를 저장하는 tree widget item 클래스
*/
class ChatLogItem : public QTreeWidgetItem
{
public:
    explicit ChatLogItem(QString="", QString="" , QString = "",\
                         QString = "", QString = "",QString="");
    QString getIP() const;
    QString getPort() const;
    QString getID() const;
    QString getName() const;
    QString getMessage() const;
    QString getTime() const;

};

#endif // CHATLOGITEM_H
