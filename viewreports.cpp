#include "viewreports.h"
#include "ui_viewreports.h"

ViewReports::ViewReports(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ViewReports)
{
    ui->setupUi(this);
}

ViewReports::~ViewReports()
{
    delete ui;
}
