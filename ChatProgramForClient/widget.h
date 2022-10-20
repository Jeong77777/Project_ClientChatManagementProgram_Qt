#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDataStream>

class QTcpSocket;
class QFile;
class QProgressDialog;

typedef enum {
    Chat_Login,             // 로그인(서버 접속)   --> 초대를 위한 정보 저장
    Chat_In,                // 채팅방 입장
    Chat_Talk,              // 채팅
    Chat_Out,             // 채팅방 퇴장         --> 초대 가능
    Chat_LogOut,            // 로그 아웃(서버 단절) --> 초대 불가능
    Chat_Invite,            // 초대
    Chat_KickOut,           // 강퇴
} Chat_Status;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    const int PORT_NUMBER = 8000;

    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void receiveData();			// 서버에서 데이터가 올 때
    void sendData();               // 서버로 데이터를 보낼 때
    void disconnect();
    void sendProtocol(Chat_Status, char*, int = 1020);
    void sendFile();
    void goOnSend(qint64);

private:
    Ui::Widget *ui;

    void closeEvent(QCloseEvent*) override;

    QTcpSocket *clientSocket;		// 클라이언트용 소켓
    QTcpSocket *fileClient;
    QProgressDialog* progressDialog;    // 파일 진행 확인
    QFile* file;
    qint64 loadSize;
    qint64 byteToWrite;
    qint64 totalSize;
    QByteArray outBlock;
    bool isSent;
};
#endif // WIDGET_H
