#ifndef CHATMANAGERFORM_H
#define CHATMANAGERFORM_H

#include <QWidget>
#include <QList>

class QLabel;
class QTcpServer;
class QTcpSocket;

typedef struct {
    int type;
    //id만듬
    char id[10];
    char data[1010];
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

private slots:
    void clientConnect();
    void echoData();
    void removeItem();

private:
    Ui::ChatManagerForm *ui;

    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientList;
};

#endif // CHATMANAGERFORM_H
