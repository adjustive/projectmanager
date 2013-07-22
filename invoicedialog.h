#ifndef INVOICEDIALOG_H
#define INVOICEDIALOG_H

#include <QDialog>
#include "projectproxymodel.h"
#include "projectview.h"

namespace Ui {
    class InvoiceDialog;
}

class InvoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InvoiceDialog(QWidget *parent = 0);
    ~InvoiceDialog();
    void setModel(ProjectProxyModel*);
    void init(QModelIndex);
    QModelIndex getProject();
    double getAmount();
    QDate getCreateDate();
    QDate getPaymentDate();
    QString getInvoiceCode();
    bool getPaid();
    QString getNotes();

private:
    Ui::InvoiceDialog *ui;
    ProjectProxyModel *modelProject;
    QModelIndex chosenIndex;

public slots:
    void setChosenIndex(QModelIndex);

};

#endif // INVOICEDIALOG_H
