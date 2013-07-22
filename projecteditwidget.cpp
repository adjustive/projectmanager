#include "projecteditwidget.h"

#include <QTabBar>
#include <QLabel>

#include <QVBoxLayout>

#include <QTextEdit>
#include <QLineEdit>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>
#include <QSqlRelationalDelegate>
#include <QDateTime>
#include <QRegExp>
#include <QRegExpValidator>

#include "projectproxymodel.h"
#include "invoiceprintdialog.h"
#include "delegates.h"

ProjectEditWidget::ProjectEditWidget(QWidget *parent) :
    QTabWidget(parent)
{

    infoTab = new QWidget(this);
    notesTab = new QWidget(this);
    paymentsTab = new QWidget(this);
    invoicesTab = new QWidget(this);

    addPaymentDialog = new PaymentEditWidget(this);
    addInvoiceDialog = new InvoiceDialog(this);

    holdTriggers = false;

    this->addTab(infoTab,"Basic Information");
    this->addTab(notesTab,"Project Notes");
    this->addTab(invoicesTab,"Invoices");
   // this->addTab(paymentsTab,"Payments");
    paymentsTab->setHidden(true);


    /// the notes tab

    QVBoxLayout *tablayout = new QVBoxLayout(notesTab);

    QVBoxLayout *notesLayout = new QVBoxLayout();
    notesLayout->addWidget(new QLabel("Project Notes") );

    notesEdit = new QTextEdit();
    notesSaveButton = new QPushButton("Save Notes");
    notesLayout->addWidget(notesEdit);
    notesLayout->addWidget(notesSaveButton);

    tablayout->addLayout(notesLayout);


    /// The payment tab

    QVBoxLayout *paytablayout = new QVBoxLayout(paymentsTab);
    QVBoxLayout *paymentLayout = new QVBoxLayout();

    paymentTable = new QTableView(this);
    paymentLayout->addWidget(paymentTable);


    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    addPaymentButton = new QPushButton("Add Payment");
    deletePaymentButton = new QPushButton("Delete Payment");
    buttonsLayout->addWidget(addPaymentButton);
    buttonsLayout->addWidget(deletePaymentButton);
    paymentLayout->addLayout(buttonsLayout);

    paytablayout->addLayout(paymentLayout);


    /// The invoices tab

    QVBoxLayout *invoiceTabLayout = new QVBoxLayout(invoicesTab);
    QVBoxLayout *invoiceLayout = new QVBoxLayout();

    invoiceTable = new QTableView(this);
    invoiceTable->setItemDelegateForColumn(6, new BoolDelegate(invoiceTable));
    invoiceTable->setItemDelegateForColumn(4, new DateDelegate(invoiceTable));
    invoiceTable->setItemDelegateForColumn(7, new DateDelegate(invoiceTable));
    invoiceLayout->addWidget(invoiceTable);


    QHBoxLayout *invoiceButtonsLayout = new QHBoxLayout();

    addInvoiceButton = new QPushButton("Add Invoice");
    deleteInvoiceButton = new QPushButton("Delete Invoice");
    QPushButton *printInvoiceButton = new QPushButton("Print Invoice");
    invoiceButtonsLayout->addWidget(addInvoiceButton);
    invoiceButtonsLayout->addWidget(deleteInvoiceButton);
    invoiceButtonsLayout->addWidget(printInvoiceButton);
    invoiceLayout->addLayout(invoiceButtonsLayout);

    invoiceTabLayout->addLayout(invoiceLayout);


    /// the project details tab

    QVBoxLayout *editLayout = new QVBoxLayout(infoTab);

    QVBoxLayout *nameLayout = new QVBoxLayout();
    nameLayout->addWidget(new QLabel("Project Name") );
    nameEdit = new QLineEdit();
    nameLayout->addWidget(nameEdit);

    QVBoxLayout *clientLayout = new QVBoxLayout();
    clientLayout->addWidget(new QLabel("Client") );
    clientEdit = new QComboBox();
    clientLayout->addWidget(clientEdit);


    QHBoxLayout *minutesRateLayout = new QHBoxLayout();

    QVBoxLayout *minutesLayout = new QVBoxLayout();
    minutesLayout->addWidget(new QLabel("Minutes") );
    minutesEdit = new QSpinBox();
    minutesEdit->setMaximum(1000000);
    minutesLayout->addWidget(minutesEdit);

    QVBoxLayout *rateLayout = new QVBoxLayout();
    rateLayout->addWidget(new QLabel("Rate") );
    rateEdit = new QSpinBox();
    rateEdit->setMaximum(1000000);
    rateLayout->addWidget(rateEdit);

    minutesRateLayout->addLayout(minutesLayout);
    minutesRateLayout->addLayout(rateLayout);


    QHBoxLayout *startEndLayout = new QHBoxLayout();

    QVBoxLayout *startLayout = new QVBoxLayout();
    startLayout->addWidget(new QLabel("Project Start") );
    startEdit = new QDateEdit();
    startEdit->setDisplayFormat("dd MMM yyyy");
    startLayout->addWidget(startEdit);

    QVBoxLayout *endLayout = new QVBoxLayout();
    endLayout->addWidget(new QLabel("Project End") );
    endEdit = new QDateEdit();
    endEdit->setDisplayFormat("dd MMM yyyy");
    endLayout->addWidget(endEdit);

    startEndLayout->addLayout(startLayout);
    startEndLayout->addLayout(endLayout);

    QHBoxLayout *finishedLayout = new QHBoxLayout();
    finishedEdit = new QCheckBox();
    finishedLayout->addWidget(finishedEdit);
    finishedLayout->addWidget(new QLabel("Project Finished") );


    QHBoxLayout *setPriceLayout = new QHBoxLayout();
    setPriceLayout->addWidget(new QLabel("Set Price") );
    setPriceEdit = new QLineEdit();
    QValidator *inputRange = new QRegExpValidator(QRegExp("(\\d{1,6}(\\.\\d\\d?)?)?"),this);
    setPriceEdit->setValidator(inputRange);

    setPriceLayout->addWidget(setPriceEdit);


    editLayout->addLayout(nameLayout);
    editLayout->addLayout(clientLayout);
    editLayout->addLayout(minutesRateLayout);
    editLayout->addLayout(startEndLayout);
    editLayout->addLayout(setPriceLayout);
    editLayout->addLayout(finishedLayout);


    connect(nameEdit, SIGNAL(editingFinished()), this, SLOT(updateProjectName()) );
    connect(setPriceEdit, SIGNAL(editingFinished()), this, SLOT(updateProjectSetPrice()) );
    connect(startEdit, SIGNAL(editingFinished()), this, SLOT(updateProjectStart()) );
    connect(endEdit, SIGNAL(editingFinished()), this, SLOT(updateProjectEnd()) );
    connect(minutesEdit, SIGNAL(editingFinished()), this, SLOT(updateProjectMinutes()) );
    connect(rateEdit, SIGNAL(editingFinished()), this, SLOT(updateProjectRate()) );
    connect(finishedEdit, SIGNAL(stateChanged(int)), this, SLOT(updateProjectFinished()) );
    connect(notesSaveButton, SIGNAL(clicked()), this, SLOT(updateProjectNotes()) );
    connect(clientEdit, SIGNAL(activated(int)), this, SLOT(updateProjectClient()) );
    connect(addPaymentButton, SIGNAL(clicked()), this, SLOT(addProjectPayment()) );
    connect(deletePaymentButton, SIGNAL(clicked()), this, SLOT(deleteProjectPayment()) );
    connect(addInvoiceButton, SIGNAL(clicked()), this, SLOT(addProjectInvoice()) );
    connect(printInvoiceButton, SIGNAL(clicked()), this, SLOT(printInvoice()) );
    connect(deleteInvoiceButton, SIGNAL(clicked()), this, SLOT(deleteProjectInvoice()) );

}

ProjectEditWidget::~ProjectEditWidget()
{
}


void ProjectEditWidget::init(ProjectProxyModel *model, ProjectView* modelList, ClientModel* clientModel, PaymentModel* payment, InvoiceModel* invoice)
{

    this->model = model;
    this->modelList = modelList;
    this->clientModel = clientModel;
    this->paymentModel = payment;
    this->invoiceModel = invoice;

    clientEdit->setModel(clientModel);
    clientEdit->setModelColumn(1);

    paymentTable->setModel(this->paymentModel);
    paymentTable->hideColumn(0);
    paymentTable->setItemDelegate(new QSqlRelationalDelegate(paymentTable));

    addPaymentDialog->setModel(this->model);

    invoiceTable->setModel(this->invoiceModel);
    invoiceTable->hideColumn(0);
    invoiceTable->setItemDelegate(new QSqlRelationalDelegate(invoiceTable));

    addInvoiceDialog->setModel(this->model);


    connect(modelList, SIGNAL(clicked(QModelIndex)), this, SLOT(populateFields(QModelIndex)));
    connect(modelList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showCustomContextMenu(QPoint)));
    connect(model, SIGNAL(variantUpdated(int,QString,QVariant)), this, SLOT(updateData(int,QString, QVariant)));
    this->setDisabled(true);
}

void ProjectEditWidget::populateFields(QModelIndex index)
{
    this->setEnabled(true);
    const QSortFilterProxyModel* proxy = static_cast<const QSortFilterProxyModel*>(index.model());
    currentModelIndex = proxy->mapToSource(index);
    QSqlRecord record = model->getRecord(currentModelIndex);
    currentModelId = record.value("id").toInt();

    this->holdTriggers = true;

    this->nameEdit->setText(record.value("project_name").toString());
    this->setPriceEdit->setText(record.value("project_set_price").toString());
    this->startEdit->setDate(record.value("project_start").toDate());
    this->endEdit->setDate(record.value("project_end").toDate());
    this->minutesEdit->setValue(record.value("project_minutes").toInt());
    this->rateEdit->setValue(record.value("project_rate").toInt());
    this->finishedEdit->setChecked(record.value("project_finished").toBool());
    this->notesEdit->setText(record.value("project_notes").toString());
    this->clientEdit->setCurrentIndex(this->clientEdit->findText(record.value("client_name").toString()));
    this->clientEdit->setEnabled(record.value("parent_id").isNull());
    this->holdTriggers = false;

    QString filter = "project_id in (";

    QList<int>* desc= model->getDescendants(currentModelId);
    for (int i = 0; i < desc->size() ; i ++ )
    {
        filter.append(QString::number(desc->at(i)));
        if ( i != desc->size() -1 )
        {
            filter.append(",");
        }
    }

    filter.append(")");
    this->paymentModel->setFilter(filter);
    this->invoiceModel->setFilter(filter);
}

void ProjectEditWidget::updateProjectClient()
{
    if (holdTriggers) return;
   // qDebug(clientEdit->currentText().toUtf8());
     model->setValue(currentModelId, "client_id", QVariant(clientModel->getId(clientEdit->currentText())));
}

void ProjectEditWidget::updateProjectName()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_name", QVariant(nameEdit->text()));
}

void ProjectEditWidget::updateProjectSetPrice()
{
    if (holdTriggers) return;

    if ( setPriceEdit->text().length() == 0)
    {
        model->setValue(currentModelId, "project_set_price", QVariant(QVariant::Double));
    }
    else
        model->setValue(currentModelId, "project_set_price", QVariant(setPriceEdit->text()));
}

void ProjectEditWidget::updateProjectStart()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_start", QVariant(startEdit->date()));
}

void ProjectEditWidget::updateProjectEnd()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_end", QVariant(endEdit->date()));
}

void ProjectEditWidget::updateProjectMinutes()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_minutes", QVariant(minutesEdit->value()));
}

void ProjectEditWidget::updateProjectRate()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_rate", QVariant(rateEdit->value()));
}

void ProjectEditWidget::updateProjectFinished()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_finished", QVariant(finishedEdit->checkState()));
    if ( finishedEdit->checkState() )
    { //changed to finished (so update end date)
        QSqlRecord record = model->getRecord(currentModelId);
        if ( record.value("project_end").toDate() == record.value("project_start").toDate())
        {
            model->setValue(currentModelId, "project_end", QVariant(QDate::currentDate()));
            endEdit->setDate(QDate::currentDate());
        }
    }
}

void ProjectEditWidget::archiveProject()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }
    model->setValue(currentModelId, "project_archived", QVariant(!model->getRecord(currentModelId).value("project_archived").toBool()));
}

void ProjectEditWidget::updateProjectNotes()
{
    if (holdTriggers) return;
    model->setValue(currentModelId, "project_notes", QVariant(notesEdit->toPlainText()));
}

void ProjectEditWidget::addSubProject()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    QSqlRecord parent = this->model->getRecord(this->currentModelId, Qt::EditRole);
    this->model->add( parent.value("client_id").toInt(),this->currentModelId,0,parent.value("project_rate").toInt(),QDate::currentDate(), QDate::currentDate(), "Enter notes here", false, false, parent.value("project_name").toString().append(" - New SubProject"),0);

}


void ProjectEditWidget::deleteProject()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("Permanently deleting a project.");
    msgBox.setInformativeText("Are you sure you want to proceed?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        this->model->deleteProject(this->currentModelId);
        this->setDisabled(true);
    }
}

void ProjectEditWidget::showCustomContextMenu(QPoint pos)
{
    if ( modelList->indexAt(pos).isValid())
    {
        this->populateFields(modelList->indexAt(pos));
        QMenu* contextMenu = new QMenu(modelList);
        contextMenu->addAction("Add Subproject",this, SLOT(addSubProject()));
        contextMenu->addAction("(Un)archive Project",this, SLOT(archiveProject()));
        contextMenu->addAction("Start",this, SLOT(activateProject()));
        contextMenu->addAction("Stop",this, SLOT(deactivateProject()));
        contextMenu->addAction("Delete",this, SLOT(deleteProject()));
        contextMenu->exec(modelList->viewport()->mapToGlobal(pos));
    }
}

void ProjectEditWidget::activateProject()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }
    model->activateProject(currentModelId);
}

void ProjectEditWidget::deactivateProject()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }
    model->deactivateProject(currentModelId);
}

void ProjectEditWidget::addProjectPayment()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    addPaymentDialog->init(this->currentModelIndex);
    bool ret = addPaymentDialog->exec();

    if (ret == true)
    {
       int id= paymentModel->add(this->model->getRecord(addPaymentDialog->getProject()).value("id").toInt(), addPaymentDialog->getAmount(), addPaymentDialog->getDate(), addPaymentDialog->getNotes());

       qDebug(QString::number(id).toUtf8());
    }

}

void ProjectEditWidget::deleteProjectPayment()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("Permanently deleting a payment.");
    msgBox.setInformativeText("Are you sure you want to proceed?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int answer = msgBox.exec();
    if (answer == QMessageBox::Yes)
    {
        this->paymentModel->removeRow(this->paymentTable->currentIndex().row());
    }


}

void ProjectEditWidget::addProjectInvoice()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    addInvoiceDialog->init(this->currentModelIndex);
    bool ret = addInvoiceDialog->exec();

    if (ret == true)
    {
        int id= invoiceModel->add(this->model->getRecord(addInvoiceDialog->getProject()).value("id").toInt(), addInvoiceDialog->getInvoiceCode(), addInvoiceDialog->getAmount(), addInvoiceDialog->getCreateDate(), addInvoiceDialog->getNotes(), addInvoiceDialog->getPaid(), addInvoiceDialog->getPaymentDate());

       qDebug(QString::number(id).toUtf8());
    }

}

void ProjectEditWidget::deleteProjectInvoice()
{
    if (!model->getModelItemById(this->currentModelId) || !this->isEnabled())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a project from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("Permanently deleting an invoice.");
    msgBox.setInformativeText("Are you sure you want to proceed?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int answer = msgBox.exec();
    if (answer == QMessageBox::Yes)
    {
        this->invoiceModel->removeRow(this->invoiceTable->currentIndex().row());
    }


}

void ProjectEditWidget::updateData(int id,QString name, QVariant value)
{
    if ( this->currentModelId != id)
    {
        return;
    }

    this->holdTriggers = true;

    if (name == "name")
    {
        this->nameEdit->setText( value.toString() );
    }

    if (name == "minutes")
    {
        this->minutesEdit->setValue( value.toInt() );
    }

    if (name == "finished")
    {
        this->finishedEdit->setChecked( value.toBool() );
    }

    if ( name == "rate" )
    {
        this->rateEdit->setValue(value.toInt());
    }

    if ( name == "client" )
    {
        this->clientEdit->setCurrentIndex(this->clientEdit->findText(value.toString()));
    }

    if ( name == "parent" )
    {
        this->clientEdit->setEnabled(value.isNull());
    }

    if ( name == "fixed price" )
    {
        this->setPriceEdit->setText(value.toString());
    }

    this->holdTriggers = false;
}


void ProjectEditWidget::printInvoice()
{
    QModelIndex idx = this->invoiceTable->currentIndex();
    if (!idx.isValid())
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose an invoice from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }
    QString str = this->invoiceModel->print(idx.row());

    QString filename = this->invoiceModel->getFilename(idx.row());
    qDebug( QString::number(idx.row()).toUtf8());
    InvoicePrintDialog dialog(str, filename, this );
    dialog.exec();

}
