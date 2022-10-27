#include "chatserverform.h"
#include "ui_chatserverform.h"
#include "logthread.h"
#include "chatwindowforadmin.h"

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
#include <QNetworkInterface> //ip확인용


ChatServerForm::ChatServerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatServerForm), totalSize(0), byteReceived(0)
{
    ui->setupUi(this); // 현재 클래스 위에 ui를 올려줌
    QList<int> sizes;
    sizes << 120 << 500;
    ui->splitter->setSizes(sizes); // split 사이즈를 정함

    ui->clientTreeWidget->QTreeView::setColumnWidth(0,70);
    ui->clientTreeWidget->QTreeView::setColumnWidth(1,50);

    chatServer = new QTcpServer(this); // tcpserver를 만들어줌
    connect(chatServer, SIGNAL(newConnection()), SLOT(clientConnect()));
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(chatServer->errorString()));
        close( );
        return;
    }

//
//    QList<QHostAddress> addrList = QNetworkInterface::allAddresses();

//    foreach(QHostAddress addr, addrList)
//    {
//        if (0 < addr.toIPv4Address())
//        {
//            qDebug() << addr.toString();
//            break;
//        }
//    }
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

    // 챗 서버와 파일 서버를 따로 만들어주었음.
    // 파일 전송은 시간이 많이 걸림
    // 파일 전송이 되는 동안 채팅이 되기 위해서 (이원화)


    qDebug("Start listening ...");

    QAction* openAction = new QAction(tr("&Open chat window"));
    openAction->setObjectName("Open");
    connect(openAction, SIGNAL(triggered()), SLOT(openChatWindow()));

    QAction* inviteAction = new QAction(tr("&Invite"));
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    QAction* removeAction = new QAction(tr("&Kick out"));
    connect(removeAction, SIGNAL(triggered()), SLOT(kickOut()));

    menu = new QMenu;
    menu->addAction(openAction);
    menu->addAction(inviteAction);
    menu->addAction(removeAction);
    ui->clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    // 로그를 쓰레드기반으로 만들었음.
    logThread = new LogThread(this);
    logThread->start();

    connect(ui->savePushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));

    qDebug() << tr("The server is running on port %1.").arg(chatServer->serverPort( ));
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

                    clientIdSocketHash[row[0]] = clientConnection;
                    portClientIdHash[port] = row[0];
                    sendLoginResult(clientConnection, "permit");

                    if(clientIdWindowHash.contains(item->text(1)))
                        clientIdWindowHash[item->text(1)]->updateInfo("", tr("Online"));
                    return;
                }

            }
        }
        sendLoginResult(clientConnection, "forbid");
        //clientConnection->disconnectFromHost();
    }
        break;
    case Chat_In:
        foreach(auto item, ui->clientTreeWidget->findItems(strData, Qt::MatchFixedString, 1)) {
            if(item->text(0) != tr("Chat in")) {
                item->setText(0, tr("Chat in"));
                item->setIcon(0, QIcon(":/images/Green-Circle.png"));

                if(clientIdWindowHash.contains(item->text(1)))
                    clientIdWindowHash[item->text(1)]->updateInfo("", tr("Chat in"));
                qDebug() << item->text(1);
            }
            //portClientIdHash[port] = id;
            if(clientIdSocketHash.contains(strData))
                clientIdSocketHash[strData] = clientConnection;
        }
        break;
    case Chat_Talk: {
        clientIdWindowHash[portClientIdHash[port]]->receiveMessage(strData);

        // 로그 남기기
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip+"("+QString::number(port)+")"); // Sender IP(Port)
        item->setText(1, portClientIdHash[port]+"("+clientIdNameHash[portClientIdHash[port]]+")"); // Sende ID(Name)
        item->setText(2, QString(data)); // message
        item->setText(3, chatServer->serverAddress().toString()+"("+QString::number(PORT_NUMBER)+")" ); // Receiver IP(Port)
        item->setText(4, QString("10000")+"("+tr("Admin")+")"); // Receiver ID(name) = 10000(Admin)
        item->setText(5, QDateTime::currentDateTime().toString()); // Time
        item->setToolTip(2, QString(data));
        ui->messageTreeWidget->addTopLevelItem(item);

        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        logThread->appendData(item);
    }
        break;
    case Chat_Out:
        foreach(auto item, ui->clientTreeWidget->findItems(strData, Qt::MatchFixedString, 1)) {
            if(item->text(0) != tr("Online")) {
                item->setText(0, tr("Online"));
                item->setIcon(0, QIcon(":/images/Blue-Circle.png"));

                if(clientIdWindowHash.contains(item->text(1)))
                    clientIdWindowHash[item->text(1)]->updateInfo("", tr("Online"));
            }
            //clientNameHash.remove(port);
        }
        break;
    case Chat_LogOut:
        qDebug()<<"log out....";
        foreach(auto item, ui->clientTreeWidget->findItems(strData, Qt::MatchFixedString, 1)) {
            qDebug() <<"loop";
            if(item->text(0) != tr("Offline")) {
                item->setText(0, tr("Offline"));
                item->setIcon(0, QIcon(":/images/Red-Circle.png"));

                if(clientIdWindowHash.contains(item->text(1)))
                    clientIdWindowHash[item->text(1)]->updateInfo("", tr("Log Out"));
            }
            if(clientIdWindowHash.contains(portClientIdHash[port]))
                clientIdWindowHash[portClientIdHash[port]]->updateInfo("", tr("Offline"));
            clientIdSocketHash.remove(portClientIdHash[port]);
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
        foreach(auto item, ui->clientTreeWidget->findItems(id, Qt::MatchFixedString, 1)) {
            qDebug() << item->text(2);
            item->setText(0, tr("Offline"));
            item->setIcon(0, QIcon(":/images/Red-Circle.png"));
            if(clientIdWindowHash.contains(id))
                clientIdWindowHash[id]->updateInfo("", tr("Offline"));
        }
        clientIdSocketHash.remove(id);
        clientConnection->deleteLater();
    }
}

void ChatServerForm::addClient(int intId, QString name)
{
    QString id = QString::number(intId);
    foreach(auto c, ui->clientTreeWidget->findItems(id, Qt::MatchFixedString, 1)) {
        c->setText(2, name);
        clientIdNameHash[id] = name;
        if(clientIdWindowHash.contains(id))
            clientIdWindowHash[id]->updateInfo(name, "");
        return;
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->clientTreeWidget);
    item->setText(0, tr("Offline"));
    item->setIcon(0, QIcon(":/images/Red-Circle.png"));
    item->setText(1, id);
    item->setText(2, name);
    ui->clientTreeWidget->addTopLevelItem(item);
    clientIdNameHash[id] = name;
    //ui->clientTreeWidget->resizeColumnToContents(0);
}

void ChatServerForm::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->clientTreeWidget->currentItem() == nullptr) return;

    foreach(QAction *action, menu->actions()) {
        if(action->objectName() == "Open")
            action->setEnabled(true);
        else if(action->objectName() == "Invite")        // 초대
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

    if(ui->clientTreeWidget->currentItem() != nullptr) {
        QString id = ui->clientTreeWidget->currentItem()->text(1);
        QTcpSocket* sock = clientIdSocketHash[id];
        sock->write(sendArray);

        ui->clientTreeWidget->currentItem()->setText(0, tr("Online"));
        ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/images/Blue-Circle.png"));

        if(clientIdWindowHash.contains(id))
            clientIdWindowHash[id]->updateInfo("", tr("Online"));
    }
}

/* 클라이언트 초대하기 */
void ChatServerForm::inviteClient()
{
    if(ui->clientTreeWidget->currentItem() == nullptr) return;

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

    if(clientIdWindowHash.contains(id))
        clientIdWindowHash[id]->updateInfo("", tr("Chat in"));
}

void ChatServerForm::inviteClienttInChatWindow(QString id)
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Invite;
    out.writeRawData("", 1020);

    /* 소켓은 현재 선택된 아이템에 표시된 ID와 해쉬로 부터 가져온다. */
    id = ui->clientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientIdSocketHash[id];
    sock->write(sendArray);

    ui->clientTreeWidget->currentItem()->setText(0, tr("Chat in"));
    ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/images/Green-Circle.png"));

    if(clientIdWindowHash.contains(id))
        clientIdWindowHash[id]->updateInfo("", tr("Chat in"));
}

void ChatServerForm::kickOutInChatWindow(QString id)
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_KickOut;
    out.writeRawData("", 1020);

    id = ui->clientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientIdSocketHash[id];
    sock->write(sendArray);

    ui->clientTreeWidget->currentItem()->setText(0, tr("Online"));
    ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/images/Blue-Circle.png"));

    if(clientIdWindowHash.contains(id))
        clientIdWindowHash[id]->updateInfo("", tr("Online"));
}

void ChatServerForm::openChatWindow()
{
    QString id = ui->clientTreeWidget->currentItem()->text(1);
    QString state;
    if(false == clientIdWindowHash.contains(id)) {
        foreach(auto item, ui->clientTreeWidget->findItems(id, Qt::MatchFixedString, 1)) {
                state = item->text(0);
            }
        ChatWindowForAdmin* w = new ChatWindowForAdmin(id, clientIdNameHash[id], state);
        clientIdWindowHash[id] = w;
        w->setWindowTitle(id +" " + clientIdNameHash[id] + " | " + state);
        w->show();
        connect(w, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendData(QString, QString)));
        connect(w, SIGNAL(inviteClient(QString)), this, SLOT(inviteClienttInChatWindow(QString)));
        connect(w, SIGNAL(kickOutClient(QString)), this, SLOT(kickOutInChatWindow(QString)));
    }
    else {
        clientIdWindowHash[id]->showNormal();
        clientIdWindowHash[id]->activateWindow();
    }
}

/* 파일 전송을 위한 소켓 생성 */
void ChatServerForm::acceptConnection()
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection(); // 다음에 기다리고잇는 커넥션
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
        item->setText(0, ip+"("+QString::number(port)+")"); // Sender IP(Port)
        item->setText(1, portClientIdHash[port]+"("+clientIdNameHash[portClientIdHash[port]]+")"); // Sende ID(Name)
        item->setText(2, filename); // filename - message
        item->setText(3, fileServer->serverAddress().toString()+"("+QString::number(PORT_NUMBER+1)+")" ); // Receiver IP(Port)
        item->setText(4, QString("10000")+"("+tr("Admin")+")"); // Receiver ID(name) = 10000(Admin)
        item->setText(5, QDateTime::currentDateTime().toString()); // Time
        item->setToolTip(2, filename);

        /* 컨텐츠의 길이로 QTreeWidget의 헤더의 크기를 고정 */
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        ui->messageTreeWidget->addTopLevelItem(item);
        logThread->appendData(item);

        QFileInfo info(filename); // 파일의 정보를 가져옴
        QString currentFileName = info.fileName(); // 파일의 경로에서 이름만 뽑아옴
        file = new QFile(currentFileName);
        file->open(QFile::WriteOnly|QFile::Truncate); //truncate 추가햇음
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

void ChatServerForm::sendLoginResult(QTcpSocket* sock, const char* msg)
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Login;
    out.writeRawData(msg, 1020);
    sock->write(sendArray);
}

void ChatServerForm::sendData(QString id, QString str)
{   
    /* 소켓은 현재 선택된 아이템에 표시된 ID와 해쉬로 부터 가져온다. */
    if(false == clientIdSocketHash.contains(id))
        return;

    QTcpSocket* sock = clientIdSocketHash[id];
    QString data;
    QByteArray sendArray;
    sendArray.clear();
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Talk;
    data = "<font color=blue>" + tr("Admin") + "</font> : " + str;
    out.writeRawData(data.toStdString().data(), 1020);
    sock->write(sendArray);

    QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
    item->setText(0, chatServer->serverAddress().toString()+"("+QString::number(PORT_NUMBER)+")" ); // Sender IP(Port)
    item->setText(1, QString("10000")+"("+tr("Admin")+")"); // Sender ID(name) = 10000(Admin)
    item->setText(2, str); // message
    item->setText(3, sock->peerAddress().toString()+"("+QString::number(sock->peerPort())+")"); // Receiver IP(Port)
    item->setText(4, id+"("+clientIdNameHash[id]+")"); // Receiver ID(name)
    item->setText(5, QDateTime::currentDateTime().toString()); // Time
    item->setToolTip(2, str);
    ui->messageTreeWidget->addTopLevelItem(item);

    for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
        ui->messageTreeWidget->resizeColumnToContents(i);

    logThread->appendData(item);
}

