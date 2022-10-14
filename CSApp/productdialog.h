#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>

class ProductItem;

namespace Ui {
class ProductDialog;
}

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr);
    ~ProductDialog();

private slots:
    void on_pushButton_clicked();

public slots:
    void receiveProductInfo(ProductItem *);

signals:
    void sendWord(QString);

public:
    Ui::ProductDialog *ui;
};

#endif // PRODUCTDIALOG_H
