#include "clientwindow.h"
#include "ui_clientwindow.h"
#include <QMessageBox>

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
}

ClientWindow::~ClientWindow()
{
  //  delete model; //deleted in mainwindow
    delete ui;
}

void ClientWindow::init(ClientModel *client)
{
    model = client;
    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);

    connect(ui->addClient, SIGNAL(clicked()), this, SLOT(addClient()));
    connect(ui->deleteClient, SIGNAL(clicked()), this, SLOT(deleteClient()));
}

void ClientWindow::closeEvent(QCloseEvent *e)
{
    emit closed();
    QMainWindow::closeEvent(e);
}

void ClientWindow::addClient()
{
    QString clientName = ui->newClientName->text();

    if (clientName.length() == 0)
        return;

    model->add(clientName);
}


void ClientWindow::deleteClient()
{

    if ( !ui->tableView->currentIndex().isValid() )
    {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("Please choose a client from the list.");
        msgBox->exec();
        delete msgBox;
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("Permanently deleting a Client.");
    msgBox.setInformativeText("Are you sure you want to proceed?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();


    if (ret == QMessageBox::Yes)
    {
        this->model->removeRow(ui->tableView->currentIndex().row());
    }

}
