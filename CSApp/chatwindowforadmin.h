#ifndef CHATWINDOWFORADMIN_H
#define CHATWINDOWFORADMIN_H

#include <QWidget>

namespace Ui {
class ChatWindowForAdmin;
}

class ChatWindowForAdmin : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindowForAdmin(QString = "0", QString = "", QString = "", QWidget *parent = nullptr);
    ~ChatWindowForAdmin();

    void receiveMessage(QString);
    void updateInfo(QString, QString);

private:
    Ui::ChatWindowForAdmin *ui;
    QString clientId;
    QString clientName;
    QString clientState;
    void changeButtonAndEditState(QString);

signals:
    void sendMessage(QString, QString);
    void inviteClient(QString);
    void kickOutClient(QString);

private slots:
    void on_inputLineEdit_returnPressed();
    void on_sendPushButton_clicked();
    void on_connectPushButton_clicked();
};

#endif // CHATWINDOWFORADMIN_H
