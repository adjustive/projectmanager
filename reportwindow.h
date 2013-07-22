#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include <QMainWindow>
#include <report.h>
#include "clientmodel.h"
#include "projectproxymodel.h"

namespace Ui {
    class ReportWindow;
}

class ReportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportWindow(QWidget *parent = 0);
    void setModels(ClientModel *clients, ProjectProxyModel *projects);

    ~ReportWindow();

private:
    Report* report;
    QString filterString;
    Ui::ReportWindow *ui;
    QSortFilterProxyModel* projectFilter;

private slots:
    void filterProjectList();

public slots:
    void quickBalanceReport();
    void reportAccordingToParameters();
};

#endif // REPORTWINDOW_H
