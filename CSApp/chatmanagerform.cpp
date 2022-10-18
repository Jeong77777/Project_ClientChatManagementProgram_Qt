#include "chatmanagerform.h"
#include "ui_chatmanagerform.h"
#include "chatlogitem.h"
#include "clientitem.h"

#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>

#define BLOCK_SIZE      1024


ChatManagerForm::ChatManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatManagerForm)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), SLOT(clientConnect()));
    if (!tcpServer->listen(QHostAddress::Any, 8000)) {
        QMessageBox::critical(this, tr("Echo Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(tcpServer->errorString( )));
        close( );
        return;
    }

    ui->stateLineEdit->setText(tr("The server is running on port %1.")
                               .arg(tcpServer->serverPort()));
    ui->portLineEdit->setText(QString::number(tcpServer->serverPort()));
    ui->ipLineEdit->setText(tcpServer->serverAddress().toString());
    qDebug() << tcpServer->serverAddress().toString();
    qDebug() <<tcpServer->serverPort();

}

ChatManagerForm::~ChatManagerForm()
{
    delete ui;
}

void ChatManagerForm::clientConnect( )
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection( );
    connect(clientConnection, SIGNAL(readyRead()), SLOT(echoData()));
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeItem()));
    ui->stateLineEdit->setText("new connection is established...");

    clientList.append(clientConnection);        // QList<QTcpSocket*> clientList;
}

void ChatManagerForm::echoData( )
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    foreach(QTcpSocket *sock, clientList) {
        if(sock != clientConnection)
            sock->write(bytearray);
    }
    //ui->stateLineEdit->setText(QString(bytearray));

    // log에 쓰기

    chatProtocolType data;
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1020);

    qDebug( ) << data.type;

    QString name, ip;
    int id;

    switch(data.type) {
    case 1:
        name = data.data;
        ip = clientConnection->peerAddress().toString();
        id = name.toInt();
        ipidClient[ip] = id;
        break;
    case 2:
        ip = clientConnection->peerAddress().toString();
        id = ipidClient[ip];
        QString port = QString::number(clientConnection->peerPort());
        QString name = "name";
        QString message = data.data;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        ChatLogItem *log = new ChatLogItem(ip, port, QString::number(id), name, message, time);
        ui->chatLogTreeWidget->addTopLevelItem(log);
        break;
    };


    /*
    QString ip = clientConnection->peerAddress().toString();
    QString port = QString::number(clientConnection->peerPort());
    QString id = "00";
    QString name = "name";
    QString message = QString(bytearray);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    ChatLogItem *log = new ChatLogItem(ip, port, id, name, message, time);
    ui->chatLogTreeWidget->addTopLevelItem(log);
    */

}

void ChatManagerForm::removeItem()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    clientList.removeOne(clientConnection);
    clientConnection->deleteLater();
}

void ChatManagerForm::clearAllList()
{
    ui->allClientTreeWidget->clear();
}
