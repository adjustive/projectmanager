#ifndef INVOICEPRINTDIALOG_H
#define INVOICEPRINTDIALOG_H

#include <QDialog>

namespace Ui {
    class InvoicePrintDialog;
}

class InvoicePrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InvoicePrintDialog(QString str, QString filename, QWidget *parent = 0);
    ~InvoicePrintDialog();

private:
    Ui::InvoicePrintDialog *ui;
    QString filename;

public slots:
    void print();

};

#endif // INVOICEPRINTDIALOG_H
