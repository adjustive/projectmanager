#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

#include <QSqlRecord>
#include <QSqlField>
#include <QDate>
#include <QMessageBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidgetPage1->deleteLater();

    this->setWindowTitle("Project Manager - Oh! My Pixel Design");

    this->syncd = new Sync();
    this->settingsWindow = new SettingsWindow(syncd,this);
    this->clientWindow = new ClientWindow(this);
    this->reportWindow = new ReportWindow(this);
    this->closeable = false;

    connect(ui->actionClientEdit, SIGNAL(triggered()), this, SLOT(showClientWindow()));
    connect(ui->actionReports, SIGNAL(triggered()), this, SLOT(showReportWindow()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsWindow()));
    connect(ui->actionAddSubproject, SIGNAL(triggered()), ui->editTabs, SLOT(addSubProject()) );
    connect(ui->actionDelete, SIGNAL(triggered()), ui->editTabs, SLOT(deleteProject()) );
    connect(ui->actionStart, SIGNAL(triggered()), ui->editTabs, SLOT(activateProject()) );
    connect(ui->actionStop, SIGNAL(triggered()), ui->editTabs, SLOT(deactivateProject()));
    connect(ui->actionArchive, SIGNAL(triggered()), ui->editTabs, SLOT(archiveProject()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(fClose()));
    connect(ui->actionShow_Archived, SIGNAL(triggered(bool)), ui->treeView, SLOT(toggleArchived(bool)));

    connect(syncd, SIGNAL(syncStarted()), this, SLOT(syncStarted()));
    connect(syncd, SIGNAL(syncSuccess(int)), this, SLOT(syncSuccess()));
    connect(syncd, SIGNAL(requestFailed()), this, SLOT(syncFailed()));
    connect(syncd, SIGNAL(needToDownload()), this, SLOT(prepareForDownload()));
    connect(syncd, SIGNAL(downloadedNew()), this, SLOT(reopenDB()));
    connect(this, SIGNAL(readyForDownload()), syncd, SLOT(download()));
    connect(this, SIGNAL(reopenedDB()), syncd, SLOT(reopenedDB()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setToolTip("Project Manager");
    trayIcon->setObjectName("Project Manager");
    trayIcon->setIcon(QIcon(":/icon"));
    trayIcon->show();

    QMenu *menu = new QMenu("Menu", this);

    menu->addAction("Manage Clients", this, SLOT(showClientWindow()));
    menu->addAction("Settings", this, SLOT(showSettingsWindow()));
    menu->addSeparator();
    menu->addAction("Exit", this, SLOT(fClose()));

    trayIcon->setContextMenu(menu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(manageTraySignal(QSystemTrayIcon::ActivationReason)));

    connect(ui->filterBox, SIGNAL(textChanged(QString)), ui->treeView, SLOT(setFilter(QString)));
    connect(ui->clearFilterButton, SIGNAL(pressed()), ui->filterBox, SLOT(clear()));
    timeCounter = new Counter();
    timeCounter->start();

    this->initModels();

    QList<int> sizes;
    sizes.append(500);
    sizes.append(400);
    ui->splitter->setSizes(sizes);

    this->syncNow();

}

void MainWindow::initModels()
{
    client = new ClientModel(this);
    modelProject = new ProjectModel(this);
    projectProxy = new ProjectProxyModel(modelProject, this);
    project = new QSortFilterProxyModel(this);
    project->setSourceModel(projectProxy);

    payment = new PaymentModel(this);
    invoice = new InvoiceModel(this);

    connect(client, SIGNAL(dataChanged(QModelIndex,QModelIndex)), projectProxy, SLOT(refresh()));

    connect(&MetaModel::getInstance(), SIGNAL(revised()), this, SLOT(syncNow()));

  connect(client, SIGNAL(updated()), &MetaModel::getInstance(), SLOT(revise()) );
  connect(modelProject, SIGNAL(updated()), &MetaModel::getInstance(), SLOT(revise()) );
//    connect(client, SIGNAL(dataChanged(QModelIndex,QModelIndex)), &MetaModel::getInstance(), SLOT(revise()) );
//    connect(modelProject, SIGNAL(dataChanged(QModelIndex,QModelIndex)), &MetaModel::getInstance(), SLOT(revise()) );
    connect(invoice, SIGNAL(dataChanged(QModelIndex,QModelIndex)), &MetaModel::getInstance(), SLOT(revise()) );

    this->clientWindow->init(client);
    ui->treeView->setModel(project);
    ui->editTabs->init((ProjectProxyModel*)project->sourceModel(), ui->treeView, client, payment, invoice);

    this->reportWindow->setModels(client, projectProxy);

    connect(ui->actionNew, SIGNAL(triggered()), projectProxy, SLOT(addNewProject()) );
    connect(timeCounter, SIGNAL(triggered()), projectProxy, SLOT(minuteTrigger()));

    //connect(timeCounter, SIGNAL(triggeredAlert()), ui->treeView, SLOT(tenMinuteTrigger()));

}

void MainWindow::fClose()
{
    int count = 0;
    while ( !synchronizing.tryLock(1))
    {
        count ++;
        if (count > 5)
        {
            qDebug("Terminating");
            break;
        }
        qDebug("Waiting for sync to close");
    }
    this->closeable = true;
        close();
}

void MainWindow::toggle()
{
    if ( this->isHidden() )
        this->show();
    else
        this->hide();

}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if (closeable)
        e->accept();
    else
    {
        Settings *set = new Settings();
        if ( set->getCloseToTray() )
        {
            e->ignore();
            this->hide();
        }
        else
        {
            e->accept();
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;

    delete client;
    delete projectProxy;
    delete project;
    delete payment;
    delete invoice;
    delete modelProject;
    delete syncd;

    timeCounter->stop();
    timeCounter->wait();
    delete timeCounter;

    DB::destroyDB();

}


void MainWindow::syncNow()
{
    Settings* set = new Settings();
    if (set->getSynchronize())
        syncd->syncNow();
    delete set;
}


void MainWindow::prepareForDownload()
{
    ui->treeView->setFocus();

    this->syncDownloadDialog = new QProgressDialog("Please Wait, a new version is being downloaded from the server", NULL,0,0,this);
    this->syncDownloadDialog->show();

    disconnect(client, SIGNAL(dataChanged(QModelIndex,QModelIndex)), projectProxy, SLOT(refresh()));
    disconnect(client, SIGNAL(updated()), &MetaModel::getInstance(), SLOT(revise()) );
    disconnect(modelProject, SIGNAL(updated()), &MetaModel::getInstance(), SLOT(revise()) );
//    disconnect(client, SIGNAL(dataChanged(QModelIndex,QModelIndex)), &MetaModel::getInstance(), SLOT(revise()) );
//    disconnect(modelProject, SIGNAL(dataChanged(QModelIndex,QModelIndex)), &MetaModel::getInstance(), SLOT(revise()) );
    disconnect(invoice, SIGNAL(dataChanged(QModelIndex,QModelIndex)), &MetaModel::getInstance(), SLOT(revise()) );

    disconnect(ui->actionNew, SIGNAL(triggered()), projectProxy, SLOT(addNewProject()) );
    disconnect(timeCounter, SIGNAL(triggered()), projectProxy, SLOT(minuteTrigger()));
    disconnect(timeCounter, SIGNAL(triggered()), this, SLOT(tenMinuteTrigger()));

    delete client;
    delete projectProxy;
    delete project;
    delete payment;
    delete modelProject;
    DB::destroyDB();

    QString filename = DB::getFile();
    QDir dir;
    dir.rename(filename, filename+".bak");

    emit readyForDownload();
}

void MainWindow::reopenDB()
{
    this->syncDownloadDialog->cancel();
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Sync Downloaded");
    msgBox.setText("Downloaded new version from server");
    msgBox.exec();

    this->initModels();
    emit reopenedDB();

}

void MainWindow::showClientWindow()
{
    this->clientWindow->show();
}

void MainWindow::showSettingsWindow()
{
    this->settingsWindow->exec();
}

void MainWindow::showReportWindow()
{
    this->reportWindow->show();
}

void MainWindow::manageTraySignal(QSystemTrayIcon::ActivationReason reason)
{
    if ( reason == QSystemTrayIcon::Trigger )
        toggle();

    if ( reason == QSystemTrayIcon::Context )
        trayIcon->contextMenu()->show();

}

void MainWindow::tenMinuteTrigger()
{
    static int minutes = 0;
    minutes ++;
    if (minutes == 10)
    {
        minutes = 0;
    }
    else
        return;

    Settings* settings = new Settings();

    if ( settings->getShowNotifications() && !projectProxy->getActiveProjects().isEmpty() && this->isHidden() )
    {
        if ( projectProxy->getActiveProjects().length() > 1 )
            trayIcon->showMessage("Active projects", QString("There are ").append(QString::number(projectProxy->getActiveProjects().length())).append(" active projects."));
        else
            trayIcon->showMessage("Active Projects", "There is 1 active project.");
    }

    delete settings;
}

void MainWindow::syncStarted()
{
   // ui->statusBar->showMessage("Syncing...");
    synchronizing.tryLock();


}

void MainWindow::syncSuccess()
{
    qDebug("Synced!");
    ui->statusBar->showMessage(QString("Synced! Last sync at ").append(QTime::currentTime().toString()));
    synchronizing.unlock();

}

void MainWindow::syncFailed()
{
    qDebug("Synced!");
    ui->statusBar->showMessage("Sync failed! Make sure your username and password is correct.");
    if (this->isHidden())
    {
        trayIcon->showMessage("Sync failed!","Check your username and password");
    }
    synchronizing.unlock();

}
