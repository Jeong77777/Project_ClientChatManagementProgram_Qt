#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>

class ProductItem;
class QTreeWidgetItem;

namespace Ui {
class ProductDialog;
}

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr);
    ~ProductDialog();

    QTreeWidgetItem* getCurrentItem();
    void clearDialog();

private slots:
    void on_pushButton_clicked();

public slots:
    void receiveProductInfo(ProductItem *);

signals:
    void sendWord(QString);

private:
    Ui::ProductDialog *ui;
};

#endif // PRODUCTDIALOG_H
