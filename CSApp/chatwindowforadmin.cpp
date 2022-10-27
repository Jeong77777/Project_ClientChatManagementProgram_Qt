#include "chatwindowforadmin.h"
#include "ui_chatwindowforadmin.h"

ChatWindowForAdmin::ChatWindowForAdmin(QString id, QString name, QString state, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindowForAdmin),  clientId(id), clientName(name), clientState(state)
{
    ui->setupUi(this);

    changeButtonAndEditState(state);
}

ChatWindowForAdmin::~ChatWindowForAdmin()
{
    delete ui;
}

void ChatWindowForAdmin::on_inputLineEdit_returnPressed()
{
    QString str = ui->inputLineEdit->text();
    if(str.length()) {
        ui->messageTextEdit->append("<font color=red>" + tr("Admin") + "</font> : " + str);
        emit sendMessage(clientId, ui->inputLineEdit->text());
        ui->inputLineEdit->clear();
    }
}

void ChatWindowForAdmin::on_sendPushButton_clicked()
{
    QString str = ui->inputLineEdit->text();
    if(str.length()) {
        ui->messageTextEdit->append("<font color=red>" + tr("Admin") + "</font> : " + str);
        emit sendMessage(clientId, ui->inputLineEdit->text());
        ui->inputLineEdit->clear();
    }
}

void ChatWindowForAdmin::receiveMessage(QString strData)
{
    ui->messageTextEdit->append("<font color=blue>" + clientName + "</font> : " + strData);
}

void ChatWindowForAdmin::updateInfo(QString name, QString state)
{
    if(name.length())
        clientName = name;
    if(state.length())
        clientState = state;

    changeButtonAndEditState(state);

    setWindowTitle(clientId + " " + clientName + " | " + clientState);
}

void ChatWindowForAdmin::changeButtonAndEditState(QString state)
{
    if(state == tr("Offline")) {
        ui->sendPushButton->setDisabled(true);
        ui->inputLineEdit->setDisabled(true);
        ui->connectPushButton->setDisabled(true);
        ui->connectPushButton->setText(tr("Invite"));
    }
    else if(state == tr("Online")) {
        ui->sendPushButton->setDisabled(true);
        ui->inputLineEdit->setDisabled(true);
        ui->connectPushButton->setEnabled(true);
        ui->connectPushButton->setText(tr("Invite"));
    }
    else { // Chat in
        ui->sendPushButton->setEnabled(true);
        ui->inputLineEdit->setEnabled(true);
        ui->connectPushButton->setEnabled(true);
        ui->connectPushButton->setText(tr("Kick out"));
    }
}

void ChatWindowForAdmin::on_connectPushButton_clicked()
{
    if(ui->connectPushButton->text() == tr("Invite"))
        emit inviteClient(clientId);
    else                                // kick out
        emit kickOutClient(clientId);
}

