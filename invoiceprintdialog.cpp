#include "invoiceprintdialog.h"
#include "ui_invoiceprintdialog.h"

#include <qprinter.h>
#include <qprintdialog.h>

InvoicePrintDialog::InvoicePrintDialog(QString str, QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InvoicePrintDialog)
{
    ui->setupUi(this);
    ui->invoiceEdit->setText(str);

    this->filename = filename;

    connect(ui->printButton, SIGNAL(clicked()), this, SLOT(print()));

}

InvoicePrintDialog::~InvoicePrintDialog()
{
    delete ui;
}

void InvoicePrintDialog::print()
{
    QTextDocument *document = ui->invoiceEdit->document();
    QPrinter printer;

    printer.setDocName(this->filename);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
       return;
    document->print(&printer);
}
