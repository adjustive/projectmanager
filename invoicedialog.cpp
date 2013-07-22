#include "invoicedialog.h"
#include "ui_invoicedialog.h"
#include "projectcompleter.h"
#include "invoicemodel.h"
#include <QCompleter>

InvoiceDialog::InvoiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InvoiceDialog)
{
    ui->setupUi(this);
}

InvoiceDialog::~InvoiceDialog()
{
    delete ui;
}

void InvoiceDialog::setModel(ProjectProxyModel *project)
{
    this->modelProject = project;
    ProjectCompleter* autocompleteProject = new ProjectCompleter(project,this);
    autocompleteProject->setCompletionColumn(0);
    autocompleteProject->setCaseSensitivity(Qt::CaseInsensitive);
    autocompleteProject->setCompletionMode(QCompleter::PopupCompletion);

    ui->projectLineEdit->setCompleter(autocompleteProject);
    ui->createDateDateEdit->setDisplayFormat("dd MMM yyyy");
    ui->paymentDateDateEdit->setDisplayFormat("dd MMM yyyy");

    QValidator *inputRange = new QDoubleValidator(0.00, 1000000, 2, this);
    ui->amountLineEdit->setValidator(inputRange);

    connect(ui->projectLineEdit->completer(), SIGNAL(activated(QModelIndex)), this, SLOT(setChosenIndex(QModelIndex)));
}


void InvoiceDialog::setChosenIndex(QModelIndex idx)
{
    QAbstractProxyModel* proxyModel = qobject_cast<QAbstractProxyModel*>(ui->projectLineEdit->completer()->completionModel());
    Q_ASSERT(proxyModel != 0);

    QModelIndex index = proxyModel->mapToSource(idx);
    this->chosenIndex = index;
}

void InvoiceDialog::init(QModelIndex modelIndex)
{
    this->chosenIndex = modelIndex;
    int id = modelProject->data(modelProject->index(modelIndex.row(), 1, modelIndex.parent())).toInt();
    double amount = modelProject->getModelItemById(id)->cost;
    amount -= InvoiceModel::getTotalProjectPayments(id);
    ui->amountLineEdit->setText(QString::number(amount));
    ui->notesTextEdit->setText("");
    ui->projectLineEdit->setText(ui->projectLineEdit->completer()->pathFromIndex(modelIndex));
    ui->createDateDateEdit->setDate(QDate::currentDate());
    ui->paidCheckBox->setChecked(false);
    ui->invoiceNumberLineEdit->setText( QString::number(InvoiceModel::countYear() + 1) + "/" + QString::number(QDate::currentDate().year()) );
}

QModelIndex InvoiceDialog::getProject()
{
    return this->chosenIndex;
}

double InvoiceDialog::getAmount()
{
    return ui->amountLineEdit->text().toDouble();
}

QString InvoiceDialog::getInvoiceCode()
{
    return ui->invoiceNumberLineEdit->text();
}

QDate InvoiceDialog::getCreateDate()
{
    return ui->createDateDateEdit->date();
}

QDate InvoiceDialog::getPaymentDate()
{
    return ui->paymentDateDateEdit->date();
}

bool InvoiceDialog::getPaid()
{
    return ui->paidCheckBox->checkState();
}

QString InvoiceDialog::getNotes()
{
    return ui->notesTextEdit->toPlainText();
}
