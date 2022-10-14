#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>

class ClientItem;

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr);
    ~ClientDialog();

private slots:
    void on_pushButton_clicked();

public slots:
    void receiveClientInfo(ClientItem *);

signals:
    void sendWord(QString);

public:
    Ui::ClientDialog *ui;
};

#endif // CLIENTDIALOG_H
