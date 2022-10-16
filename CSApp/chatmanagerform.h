#ifndef CHATMANAGERFORM_H
#define CHATMANAGERFORM_H

#include <QWidget>

namespace Ui {
class ChatManagerForm;
}

class ChatManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatManagerForm(QWidget *parent = nullptr);
    ~ChatManagerForm();

private:
    Ui::ChatManagerForm *ui;
};

#endif // CHATMANAGERFORM_H
