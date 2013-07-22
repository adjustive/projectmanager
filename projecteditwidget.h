#ifndef WIDGETPROJECTEDIT_H
#define WIDGETPROJECTEDIT_H

#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QCheckBox>z
#include <QSpinBox>
#include <QPushButton>

#include "projectproxymodel.h"
#include "paymenteditwidget.h"
#include "clientmodel.h"
#include "paymentmodel.h"
#include "projectview.h"
#include "invoicedialog.h"
#include "invoicemodel.h"

class ProjectEditWidget : public QTabWidget
{
    Q_OBJECT

private:
    int currentModelId;
    QModelIndex currentModelIndex;

    QWidget* infoTab;
    QWidget* notesTab;
    QWidget* paymentsTab;
    QWidget* invoicesTab;

    PaymentEditWidget* addPaymentDialog;
    InvoiceDialog* addInvoiceDialog;

    ProjectProxyModel *model;

    ClientModel *clientModel;
    PaymentModel *paymentModel;
    InvoiceModel *invoiceModel;

    ProjectView* modelList;
    bool holdTriggers;

    QTableView* paymentTable;
    QTableView* invoiceTable;
    QPushButton* addPaymentButton;
    QPushButton* addInvoiceButton;
    QPushButton* deletePaymentButton;
    QPushButton* deleteInvoiceButton;

    QLineEdit* nameEdit;
    QLineEdit* setPriceEdit;
    QComboBox* clientEdit;
    QSpinBox* minutesEdit;
    QSpinBox* rateEdit;
    QDateEdit* startEdit;
    QDateEdit* endEdit;
    QTextEdit* notesEdit;
    QPushButton* notesSaveButton;
    QCheckBox* finishedEdit;



public:
    explicit ProjectEditWidget(QWidget *parent = 0);
    ~ProjectEditWidget();
    void init(ProjectProxyModel*, ProjectView*, ClientModel*, PaymentModel*, InvoiceModel*);

signals:

public slots:
    void populateFields(QModelIndex);
    void updateProjectName();
    void updateProjectSetPrice();
    void updateProjectStart();
    void updateProjectEnd();
    void updateProjectMinutes();
    void updateProjectRate();
    void updateProjectNotes();
    void updateProjectFinished();
    void updateProjectClient();
    void addSubProject();
    void deleteProject();
    void showCustomContextMenu(QPoint);
    void activateProject();
    void deactivateProject();
    void archiveProject();
    void addProjectPayment();
    void addProjectInvoice();
    void deleteProjectPayment();
    void deleteProjectInvoice();
    void updateData(int,QString, QVariant);
    void printInvoice();
};

#endif // WIDGETPROJECTEDIT_H
