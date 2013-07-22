#include "reportwindow.h"
#include "ui_reportwindow.h"

#include "exception.h"

ReportWindow::ReportWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportWindow)
{
    ui->setupUi(this);
    report = new Report();

    connect(ui->actionPending_Costs_Report, SIGNAL(triggered()), this, SLOT(quickBalanceReport()));
    connect(ui->generateReportButton, SIGNAL(clicked()), this, SLOT(reportAccordingToParameters()));
    connect(ui->clientList, SIGNAL(clicked(QModelIndex)), this, SLOT(filterProjectList()));

}

ReportWindow::~ReportWindow()
{
    delete report;
    delete projectFilter;
    delete ui;
}

void ReportWindow::quickBalanceReport()
{
    ui->textBrowser->setHtml(report->balanceReport(Report::ALL_DATES|Report::PER_CLIENT));
}


void ReportWindow::setModels(ClientModel *clients, ProjectProxyModel *projects)
{
    projectFilter = new QSortFilterProxyModel(this);
    projectFilter->setSourceModel(projects);
    ui->clientList->setModel(clients);
    ui->projectList->setModel(projectFilter);
    ui->clientList->setModelColumn(1);
    ui->projectList->setColumnHidden(1,true);
    ui->projectList->setColumnHidden(3,true);
    ui->projectList->setColumnHidden(4,true);
    ui->projectList->setColumnHidden(5,true);
    ui->projectList->resizeColumnToContents(0);
    projectFilter->setFilterKeyColumn(2);
}

void ReportWindow::filterProjectList()
{
    QModelIndexList selectedIndexes = ui->clientList->selectionModel()->selectedIndexes();

    QStringList filterNames;
    foreach ( QModelIndex selectedIndex, selectedIndexes)
    {
        filterNames.append(ui->clientList->model()->data(selectedIndex).toString());
    }

    QString filter = filterNames.join("|");
    projectFilter->setFilterRegExp(filter);
}

void ReportWindow::reportAccordingToParameters()
{
    int modes = 0;
    QMap<QString, QVariant> data;

    if (ui->periodReportCheckbox->checkState())
    {
        modes |= Report::PERIOD;
    }
    else
    {
        modes |= Report::ALL_DATES;
    }

    if ( ui->includeSubprojectsButton->checkState())
    {
        modes |= Report::SUBPROJECTS;
    }

    modes |= Report::PER_CLIENT;

    if ( ui->filterModeSelection->currentText() == "All Clients")
    {

    }

    if ( ui->filterModeSelection->currentText() == "All Projects")
    {

    }

    if ( ui->filterModeSelection->currentText() == "Selected")
    {
        modes |= Report::FILTERED;
        QStringList clientFilter, projectFilter;
        QModelIndexList selectedClients = ui->clientList->selectionModel()->selectedIndexes();
        foreach ( QModelIndex selectedClient, selectedClients)
        {
            clientFilter.append(ui->clientList->model()->data(ui->clientList->model()->index(selectedClient.row(), 0, selectedClient.parent())).toString());
        }

        QModelIndexList selectedProjects = ui->projectList->selectionModel()->selectedIndexes();
        foreach ( QModelIndex selectedProject, selectedProjects)
        {
            projectFilter.append(ui->projectList->model()->data(ui->projectList->model()->index(selectedProject.row(), 1, selectedProject.parent())).toString());
        }

        data["client_filter"] = clientFilter;
        data["project_filter"] = projectFilter;

    }
    QString reportText;

    try
    {
        reportText = report->balanceReport(modes, &data);
    }
    catch (Exception *e)
    {
        qDebug(e->getText().toUtf8());
    }

    ui->textBrowser->setHtml(reportText);
}
