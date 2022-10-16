#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientManagerForm;
class ProductManagerForm;
class OrderManagerForm;
class ChatManagerForm;
class QMdiSubWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionClient_triggered();
    void checkActiveSubWindow(QMdiSubWindow *);

signals:
    void chatFormActivated();

private:
    Ui::MainWindow *ui;
    ClientManagerForm *clientForm;
    ProductManagerForm *productForm;
    OrderManagerForm *orderForm;
    ChatManagerForm *chatForm;

};
#endif // MAINWINDOW_H
