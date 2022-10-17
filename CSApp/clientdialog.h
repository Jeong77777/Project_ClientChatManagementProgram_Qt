#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>

class ClientItem;
class QTreeWidgetItem;

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr);
    ~ClientDialog();

    QTreeWidgetItem* getCurrentItem();
    void clearDialog();


private slots:
    void on_pushButton_clicked();

public slots:
    void receiveClientInfo(ClientItem *);

signals:
    void sendWord(QString);

private:
    Ui::ClientDialog *ui;
};

#endif // CLIENTDIALOG_H
