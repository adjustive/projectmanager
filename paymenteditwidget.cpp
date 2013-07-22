#include "paymenteditwidget.h"
#include "ui_widgetpaymentedit.h"
#include "projectcompleter.h"

#include <QCompleter>

PaymentEditWidget::PaymentEditWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WidgetPaymentEdit)
{
    ui->setupUi(this);
}

PaymentEditWidget::~PaymentEditWidget()
{
    delete ui;
}

void PaymentEditWidget::setModel(ProjectProxyModel *project)
{
    this->modelProject = project;
    ProjectCompleter* autocompleteProject = new ProjectCompleter(project,this);
    autocompleteProject->setCompletionColumn(0);
    autocompleteProject->setCaseSensitivity(Qt::CaseInsensitive);
    autocompleteProject->setCompletionMode(QCompleter::PopupCompletion);
    ui->projectEdit->setCompleter(autocompleteProject);
    ui->dateEdit->setDisplayFormat("dd MMM yyyy");

    QValidator *inputRange = new QDoubleValidator(0.00, 1000000, 2, this);
    ui->amountEdit->setValidator(inputRange);

    connect(ui->projectEdit->completer(), SIGNAL(activated(QModelIndex)), this, SLOT(setChosenIndex(QModelIndex)));
}


void PaymentEditWidget::setChosenIndex(QModelIndex idx)
{
    QAbstractProxyModel* proxyModel = qobject_cast<QAbstractProxyModel*>(ui->projectEdit->completer()->completionModel());
    Q_ASSERT(proxyModel != 0);

    QModelIndex index = proxyModel->mapToSource(idx);
    this->chosenIndex = index;
}

void PaymentEditWidget::init(QModelIndex modelIndex)
{
    this->chosenIndex = modelIndex;
    ui->amountEdit->setText("0.00");
    ui->notesEdit->setText("");
    ui->projectEdit->setText(ui->projectEdit->completer()->pathFromIndex(modelIndex));
    ui->dateEdit->setDate(QDate::currentDate());
}

QModelIndex PaymentEditWidget::getProject()
{
    return this->chosenIndex;
}

double PaymentEditWidget::getAmount()
{
    return ui->amountEdit->text().toDouble();
}

QDate PaymentEditWidget::getDate()
{
    return ui->dateEdit->date();
}

QString PaymentEditWidget::getNotes()
{
    return ui->notesEdit->toPlainText();
}
