#ifndef WIDGETPAYMENTEDIT_H
#define WIDGETPAYMENTEDIT_H

#include <QDialog>
#include "projectproxymodel.h"
#include "projectview.h"

namespace Ui {
    class WidgetPaymentEdit;
}

class PaymentEditWidget : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentEditWidget(QWidget *parent = 0);
    ~PaymentEditWidget();
    void setModel(ProjectProxyModel*);
    void init(QModelIndex);
    QModelIndex getProject();
    double getAmount();
    QDate getDate();
    QString getNotes();

private:
    Ui::WidgetPaymentEdit *ui;
    ProjectProxyModel *modelProject;
    QModelIndex chosenIndex;

public slots:
    void setChosenIndex(QModelIndex);

};

#endif // WIDGETPAYMENTEDIT_H
