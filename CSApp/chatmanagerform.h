#ifndef CHATMANAGERFORM_H
#define CHATMANAGERFORM_H

#include <QWidget>
#include <QList>
#include <QHash>

class QLabel;
class QTcpServer;
class QTcpSocket;
class ClientItem;

typedef struct {
    int type;
    char data[1020];
} chatProtocolType;

namespace Ui {
class ChatManagerForm;
}

class ChatManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatManagerForm(QWidget *parent = nullptr);
    ~ChatManagerForm();

    void clearAllList();

private slots:
    void clientConnect();
    void echoData();
    void removeItem();

private:
    Ui::ChatManagerForm *ui;

    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientList;
    QHash<QString, int> ipidClient;
};

#endif // CHATMANAGERFORM_H
