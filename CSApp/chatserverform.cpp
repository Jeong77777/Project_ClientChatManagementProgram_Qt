#include "chatserverform.h"
#include "ui_chatserverform.h"
#include "logthread.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QNetworkInterface>

ChatServerForm::ChatServerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatServerForm), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);
    QList<int> sizes;
    sizes << 120 << 500;
    ui->splitter->setSizes(sizes);

    ui->clientTreeWidget->QTreeView::setColumnWidth(0,70);
    ui->clientTreeWidget->QTreeView::setColumnWidth(1,50);

    chatServer = new QTcpServer(this);
    connect(chatServer, SIGNAL(newConnection()), SLOT(clientConnect()));
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(chatServer->errorString()));
        close( );
        return;
    }
//
    QList<QHostAddress> addrList = QNetworkInterface::allAddresses();

    foreach(QHostAddress addr, addrList)
    {
        if (0 < addr.toIPv4Address())
        {
            qDebug() << addr.toString();
            break;
        }
    }
//
    fileServer = new QTcpServer(this);
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));
        close( );
        return;
    }

    qDebug("Start listening ...");

    QAction* inviteAction = new QAction(tr("&Invite"));
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    QAction* removeAction = new QAction(tr("&Kick out"));
    connect(removeAction, SIGNAL(triggered()), SLOT(kickOut()));

    menu = new QMenu;
    menu->addAction(inviteAction);
    menu->addAction(removeAction);
    ui->clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    logThread = new LogThread(this);
    logThread->start();

    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));

    qDebug() << tr("The server is running on port %1.").arg(chatServer->serverPort( ));

    connect(ui->inputLineEdit, SIGNAL(returnPressed()), SLOT(sendData()));
    connect(ui->inputLineEdit, SIGNAL(returnPressed()), ui->inputLineEdit, SLOT(clear()));
}

ChatServerForm::~ChatServerForm()
{
    delete ui;

    logThread->terminate();
    chatServer->close( );
    fileServer->close( );
}

void ChatServerForm::clientConnect( )
{
    QTcpSocket *clientConnection = chatServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), SLOT(receiveData()));
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeClient()));
    qDebug("new connection is established...");
}

void ChatServerForm::receiveData( )
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    Chat_Status type;       // 채팅의 목적
    char data[1020];        // 전송되는 메시지/데이터
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();
    QString strData = QString::fromStdString(data);

    qDebug() << ip << " : " << type;

    switch(type) {
    case Chat_Login: {
        QList<QString> row = strData.split(", "); // row[0] = id, row[1] = name
        foreach(auto item, ui->clientTreeWidget->findItems(row[0], Qt::MatchFixedString, 1)) {
            if(item->text(2) == row[1]) {
            if(item->text(0) != tr("Online")) {
                item->setText(0, tr("Online"));
                item->setIcon(0, QIcon(":/images/Blue-Circle.png"));
            }
            clientIdSocketHash[row[0]] = clientConnection;
            portClientIdHash[port] = row[0];
            permitLogIn(clientConnection, "permit");
            return;
            }
        }
        permitLogIn(clientConnection, "forbid");
        //clientConnection->disconnectFromHost();
    }
        break;
    case Chat_In:
        foreach(auto item, ui->clientTreeWidget->findItems(strData, Qt::MatchFixedString, 1)) {
            if(item->text(0) != tr("Chat in")) {
                item->setText(0, tr("Chat in"));
                item->setIcon(0, QIcon(":/images/Green-Circle.png"));
            }
            //portClientIdHash[port] = id;
            if(clientIdSocketHash.contains(strData))
                clientIdSocketHash[strData] = clientConnection;
        }
        break;
    case Chat_Talk: {
        foreach(QTcpSocket *sock, clientIdSocketHash.values()) {
            qDebug() << sock->peerPort();
            if(portClientIdHash.contains(sock->peerPort()) && port != sock->peerPort()) {
                foreach(auto item, ui->clientTreeWidget->findItems(portClientIdHash[sock->peerPort()], Qt::MatchFixedString, 1)) {
                    if(item->text(0) == tr("Chat in")) {
                        QByteArray sendArray;
                        sendArray.clear();
                        QDataStream out(&sendArray, QIODevice::WriteOnly);
                        out << Chat_Talk;
                        sendArray.append("<font color=blue>");
                        sendArray.append(clientIdNameHash[portClientIdHash[port]].toStdString().data());
                        sendArray.append("</font> : ");
                        sendArray.append(strData.toStdString().data());
                        sock->write(sendArray);
                        qDebug() << sock->peerPort();
                    }
                }
            }
        }

        ui->messageTextEdit->append("<font color=blue>" \
                                    + clientIdNameHash[portClientIdHash[port]] \
                                    + "</font> : " + strData);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, portClientIdHash[port]);
        item->setText(3, clientIdNameHash[portClientIdHash[port]]);
        item->setText(4, QString(data));
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, QString(data));
        ui->messageTreeWidget->addTopLevelItem(item);

        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        logThread->appendData(item);
    }
        break;
    case Chat_Out:
        foreach(auto item, ui->clientTreeWidget->findItems(strData, Qt::MatchContains, 1)) {
            if(item->text(0) != tr("Online")) {
                item->setText(0, tr("Online"));
                item->setIcon(0, QIcon(":/images/Blue-Circle.png"));
            }
            //clientNameHash.remove(port);
        }
        break;
    case Chat_LogOut:
        foreach(auto item, ui->clientTreeWidget->findItems(strData, Qt::MatchContains, 1)) {
            if(item->text(0) != tr("Offline")) {
                item->setText(0, tr("Offline"));
                item->setIcon(0, QIcon(":/images/Red-Circle.png"));
            }
            //clientSocketHash.remove(name);
            portClientIdHash.remove(port);
        }
        break;
    default:
        break;
    }
}

void ChatServerForm::removeClient()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    if(clientConnection != nullptr) {
        QString id = portClientIdHash[clientConnection->peerPort()];
        foreach(auto item, ui->clientTreeWidget->findItems(id, Qt::MatchContains, 1)) {
            item->setText(0, tr("Offline"));
            item->setIcon(0, QIcon(":/images/Red-Circle.png"));
        }
        clientIdSocketHash.remove(id);
        clientConnection->deleteLater();
    }
}

void ChatServerForm::addClient(int id, QString name)
{
    foreach(auto v, ui->clientTreeWidget->findItems(QString::number(id), Qt::MatchFixedString, 1)) {
        v->setText(2, name);
        clientIdNameHash[QString::number(id)] = name;
        return;
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->clientTreeWidget);
    item->setText(0, tr("Offline"));
    item->setIcon(0, QIcon(":/images/Red-Circle.png"));
    item->setText(1, QString::number(id));
    item->setText(2, name);
    ui->clientTreeWidget->addTopLevelItem(item);
    clientIdNameHash[QString::number(id)] = name;
    //ui->clientTreeWidget->resizeColumnToContents(0);
}

void ChatServerForm::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->clientTreeWidget->currentItem() == nullptr) return;

    foreach(QAction *action, menu->actions()) {
        if(action->objectName() == "Invite")        // 초대
            action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) == tr("Online"));
        else                                        // 강퇴
            action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) == tr("Chat in"));
    }
    QPoint globalPos = ui->clientTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

/* 클라이언트 강퇴하기 */
void ChatServerForm::kickOut()
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_KickOut;
    out.writeRawData("", 1020);

    QString id = ui->clientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientIdSocketHash[id];
    sock->write(sendArray);

    ui->clientTreeWidget->currentItem()->setText(0, tr("Online"));
    ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/images/Blue-Circle.png"));
}

/* 클라이언트 초대하기 */
void ChatServerForm::inviteClient()
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Invite;
    out.writeRawData("", 1020);

    /* 소켓은 현재 선택된 아이템에 표시된 ID와 해쉬로 부터 가져온다. */
    QString id = ui->clientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientIdSocketHash[id];
    sock->write(sendArray);


    ui->clientTreeWidget->currentItem()->setText(0, tr("Chat in"));
    ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/images/Green-Circle.png"));
}

/* 파일 전송을 위한 소켓 생성 */
void ChatServerForm::acceptConnection()
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

/* 파일 받기 */
void ChatServerForm::readClient()
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));
    QString filename, id;

    if (byteReceived == 0) {        // 파일 전송 시작 : 파일에 대한 정보를 이용해서 QFile 객체 생성
        progressDialog->reset();
        progressDialog->show();

        QString ip = receivedSocket->peerAddress().toString();
        quint16 port = receivedSocket->peerPort();
        qDebug() << ip << " : " << port;

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> id;
        progressDialog->setMaximum(totalSize);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, id);
        item->setText(3, clientIdNameHash[id]);
        item->setText(4, filename);
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, filename);

        /* 컨텐츠의 길이로 QTreeWidget의 헤더의 크기를 고정 */
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        ui->messageTreeWidget->addTopLevelItem(item);
        logThread->appendData(item);

        QFileInfo info(filename);
        QString currentFileName = info.fileName();
        file = new QFile(currentFileName);
        file->open(QFile::WriteOnly);
    } else {                    // 파일 데이터를 읽어서 저장
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }

    progressDialog->setValue(byteReceived);

    if (byteReceived == totalSize) {        /* 파일의 다 읽으면 QFile 객체를 닫고 삭제 */
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();

        file->close();
        delete file;
    }
}

void ChatServerForm::permitLogIn(QTcpSocket* sock, const char* msg)
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Login;
    out.writeRawData(msg, 1020);
    sock->write(sendArray);
}

void ChatServerForm::sendData()
{
    QString str = ui->inputLineEdit->text( );
    if(str.length( )) {
        ui->messageTextEdit->append("<font color=red>" + tr("Admin") + "</font> : " + str);

        /* 클라이언트들한테 보내기 */
        foreach(QTcpSocket *sock, clientIdSocketHash.values()) {
            qDebug() << sock->peerPort();
            foreach(auto item, ui->clientTreeWidget->findItems(portClientIdHash[sock->peerPort()], Qt::MatchFixedString, 1)) {
                if(item->text(0) == tr("Chat in")) {
                    QByteArray sendArray;
                    sendArray.clear();
                    QDataStream out(&sendArray, QIODevice::WriteOnly);
                    out << Chat_Talk;
                    sendArray.append("<font color=blue>");
                    sendArray.append(tr("Admin").toStdString().data());
                    sendArray.append("</font> : ");
                    sendArray.append(str.toStdString().data());
                    sock->write(sendArray);
                    qDebug() << sock->peerPort();
                }
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, chatServer->serverAddress().toString());
        item->setText(1, QString::number(chatServer->serverPort()));
        item->setText(2, QString::number(0));
        item->setText(3, tr("Admin"));
        item->setText(4, str);
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, str);
        ui->messageTreeWidget->addTopLevelItem(item);

        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        logThread->appendData(item);
    }
}
