#include "widget.h"
#include "ui_widget.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDataStream>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

#define BLOCK_SIZE      1024

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget), isSent(false)
{
    ui->setupUi(this);

    // 연결한 서버 정보 입력을 위한 위젯들
    QSettings settings("ChatClient", "Chat Client");
    ui->name->setText(settings.value("ChatClient/ID").toString());

    ui->serverAddress->setText("127.0.0.1");
    ui->serverAddress->setText("127.0.0.1");
    //serverAddress->setInputMask("999.999.999.999;_");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);
    ui->serverAddress->setPlaceholderText("Server IP Address");
    ui->serverAddress->setValidator(&validator);

    ui->serverPort->setText(QString::number(PORT_NUMBER));
    ui->serverPort->setInputMask("00000;_");
    ui->serverPort->setPlaceholderText("Server Port No");

    connect(ui->inputLine, SIGNAL(returnPressed()), SLOT(sendData()));
    connect(ui->inputLine, SIGNAL(returnPressed()), ui->inputLine, SLOT(clear()));

    connect(ui->sentButton, SIGNAL(clicked()), SLOT(sendData()));
    connect(ui->sentButton, SIGNAL(clicked()), ui->inputLine, SLOT(clear()));

    ui->inputLine->setDisabled(true);
    ui->sentButton->setDisabled(true);

    connect(ui->fileButton, SIGNAL(clicked()), SLOT(sendFile()));
    ui->fileButton->setDisabled(true);

    // 로그아웃 기능
    connect(ui->logOutButton, &QPushButton::clicked, this,
            [=]{
        sendProtocol(Chat_LogOut, ui->id->text().toStdString().data());
        ui->connectButton->setText(tr("Log In"));
        ui->fileButton->setDisabled(true);
        ui->sentButton->setDisabled(true);
        ui->message->clear();
        ui->id->setReadOnly(false);
        ui->name->setReadOnly(false);
    } );

    /* 채팅을 위한 소켓 */
    clientSocket = new QTcpSocket(this);			// 클라이언트 소켓 생성
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString(); });
    connect(clientSocket, SIGNAL(readyRead()), SLOT(receiveData()));
    connect(clientSocket, SIGNAL(disconnected()), SLOT(disconnect()));
    //내가만든거-에러
    connect(clientSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(connectError(QAbstractSocket::SocketError)));

    /* 파일 전송을 위한 소켓 */
    fileClient = new QTcpSocket(this);
    connect(fileClient, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));
    //    connect(fileClient, SIGNAL(disconnected( )), fileClient, SLOT(deletelater( )));

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    connect(ui->connectButton, &QPushButton::clicked,
            [=]{
        if(ui->connectButton->text() == tr("Log In")) {
            ui->connectButton->setDisabled(true);
            ui->id->setReadOnly(true);
            ui->name->setReadOnly(true);
            clientSocket->connectToHost(ui->serverAddress->text( ),
                                        ui->serverPort->text( ).toInt( ));
            clientSocket->waitForConnected();
            sendProtocol(Chat_Login, (ui->id->text() + ", " + ui->name->text()).toStdString().data());
        } else if(ui->connectButton->text() == tr("Chat in"))  {
            sendProtocol(Chat_In, ui->id->text().toStdString().data());
            ui->connectButton->setText(tr("Chat Out"));
            ui->inputLine->setEnabled(true);
            ui->sentButton->setEnabled(true);
            ui->fileButton->setEnabled(true);
        } else if(ui->connectButton->text() == tr("Chat Out"))  {
            sendProtocol(Chat_Out, ui->id->text().toStdString().data());
            ui->connectButton->setText(tr("Chat in"));
            ui->inputLine->setDisabled(true);
            ui->sentButton->setDisabled(true);
            ui->fileButton->setDisabled(true);
        }
    } );

    setWindowTitle(tr("Chat Client"));
}

Widget::~Widget()
{
    clientSocket->close( );
    QSettings settings("ChatClient", "Chat Client");
    settings.setValue("ChatClient/ID", ui->name->text());

    delete ui;
}

/* 창이 닫힐 때 서버에 연결 접속 메시지를 보내고 종료 */
void Widget::closeEvent(QCloseEvent*)
{
    sendProtocol(Chat_LogOut, ui->id->text().toStdString().data());
    clientSocket->disconnectFromHost();
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();
}

/* 데이터를 받을 때 */
void Widget::receiveData( )
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender( ));
    if (clientSocket->bytesAvailable( ) > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);

    Chat_Status type;       // 채팅의 목적
    char data[1020];        // 전송되는 메시지/데이터
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;             // 패킷의 타입
    in.readRawData(data, 1020);     // 실제 데이터

    switch(type) {
    case Chat_Login:
        qDebug() << data;
        qDebug() << "result: "<<(strcmp(data, "permit"));
        if(0 == strcmp(data, "permit")) {
            ui->connectButton->setText(tr("Chat in"));
            ui->connectButton->setEnabled(true);            
        }
        else {
            ui->connectButton->setEnabled(true);
            QMessageBox::critical(this, tr("Chatting Client"), \
                                  tr("Login failed.\nCustomer information is invalid."));
            ui->id->setReadOnly(false);
            ui->name->setReadOnly(false);
        }
        break;
    case Chat_Talk:         // 온 패킷의 타입이 대화이면
        ui->message->append(QString(data));     // 온메시지를 화면에 표시
        ui->inputLine->setEnabled(true);        // 버튼의 상태 변경
        ui->sentButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
        break;
    case Chat_KickOut:      // 강퇴면
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Kick out from Server"));
        ui->inputLine->setDisabled(true);       // 버튼의 상태 변경
        ui->sentButton->setDisabled(true);
        ui->fileButton->setDisabled(true); // 메시지 입력 불가
        ui->connectButton->setText("Chat in");
        break;
    case Chat_Invite:       // 초대면
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Invited from Server"));
        ui->inputLine->setEnabled(true);
        ui->sentButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
        ui->id->setReadOnly(true);            // 메시지 입력 가능
        ui->name->setReadOnly(true);
        ui->connectButton->setText("Chat Out");
        break;
    default:
        break;
    };
}

/* 연결이 끊어졌을 때 : 상태 변경 */
void Widget::disconnect( )
{
    QMessageBox::critical(this, tr("Chatting Client"), \
                          tr("Disconnect from Server"));
    ui->inputLine->setEnabled(false);
    ui->id->setReadOnly(false);
    ui->name->setReadOnly(false);
    ui->sentButton->setEnabled(false);
    ui->fileButton->setEnabled(false);
    ui->connectButton->setText(tr("Log In"));
}

/* 프로토콜을 생성해서 서버로 전송 */
void Widget::sendProtocol(Chat_Status type, char* data, int size)
{
    QByteArray dataArray;           // 소켓으로 보낼 데이터를 채우고
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);
    clientSocket->write(dataArray);     // 서버로 전송
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

/* 메시지 보내기 */
void Widget::sendData(  )
{
    QString str = ui->inputLine->text( );
    if(str.length( )) {
        QByteArray bytearray;
        bytearray = str.toUtf8( );
        /* 화면에 표시 : 앞에 'me(나)'라고 추가 */
        ui->message->append("<font color=red>" + tr("Me") + "</font> : " + str);
        sendProtocol(Chat_Talk, bytearray.data());
    }
}

/* 파일 전송시 여러번 나눠서 전송 */
void Widget::goOnSend(qint64 numBytes) // Start sending file content
{
    byteToWrite -= numBytes; // Remaining data size
    outBlock = file->read(qMin(byteToWrite, numBytes));
    fileClient->write(outBlock);

    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize-byteToWrite);

    if (byteToWrite == 0) { // Send completed
        qDebug("File sending completed!");
        progressDialog->reset();
    }
}

/* 파일 보내기 */
void Widget::sendFile() // Open the file and get the file name (including path)
{
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);
        file->open(QFile::ReadOnly);

        qDebug() << QString("file %1 is opened").arg(filename);
        progressDialog->setValue(0); // Not sent for the first time

        if (!isSent) { // Only the first time it is sent, it happens when the connection generates the signal connect
            fileClient->connectToHost(ui->serverAddress->text( ),
                                      ui->serverPort->text( ).toInt( ) + 1);
            isSent = true;
        }

        // When sending for the first time, connectToHost initiates the connect signal to call send, and you need to call send after the second time

        byteToWrite = totalSize = file->size(); // The size of the remaining data
        loadSize = 1024; // The size of data sent each time

        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out << qint64(0) << qint64(0) << filename << ui->id->text();

        totalSize += outBlock.size(); // The total size is the file size plus the size of the file name and other information
        byteToWrite += outBlock.size();

        out.device()->seek(0); // Go back to the beginning of the byte stream to write a qint64 in front, which is the total size and file name and other information size
        out << totalSize << qint64(outBlock.size());

        fileClient->write(outBlock); // Send the read file to the socket

        progressDialog->setMaximum(totalSize);
        progressDialog->setValue(totalSize-byteToWrite);
        progressDialog->show();
    }
    qDebug() << QString("Sending file %1").arg(filename);
}

void Widget::connectError(QAbstractSocket::SocketError)
{
//    QMessageBox::critical(this, tr("Chatting Client"), \
//                          tr("Connect error"));
//    ui->connectButton->setEnabled(true);
//    //수정해야함.
}
