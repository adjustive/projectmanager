#include "report.h"

#include "paymentmodel.h"
#include "clientmodel.h"
#include "invoicemodel.h"
#include "projectmodel.h"
#include "dbexception.h"
#include "reportexception.h"


#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QDate>
#include <QDebug>

Report::Report()
{

}




double Report::getTotalClientCost(int id)
{
    double total = 0;

    QString filter = "";

    ProjectModel* project = new ProjectModel();

    filter.append("project.parent_id is null and project.client_id = ");
    filter.append(QString::number(id));

    project->setFilter(filter);
    project->select();

    for ( int i = 0; i < project->rowCount(); i ++)
    {
        total += ProjectModel::getTotalProjectCost(project->record(i).value("id").toInt());
    }

    delete project;
    return total;
}

double Report::getTotalClientPayments(int id)
{

    QSqlQuery query(DB::get());

    QString q = QString("select SUM(inv.amount)")
            .append(" from invoice inv ")
            .append(" left join project pr on (pr.id = inv.project_id) ")
            .append(" where pr.client_id =:id and inv.paid = :paid")
            ;


    query.prepare(q);
    query.bindValue(":id",id);
    query.bindValue(":paid", true);


    bool ret = query.exec();
    double total = 0;

    if (!ret)
    {
        qDebug(query.lastError().text().toUtf8());
        qDebug(query.lastQuery().toUtf8());

    }
    else
    {
        if(query.next())
            total = query.value(0).toDouble();
    }

    return total;
}



QString Report::balanceReport(int flags, QMap<QString, QVariant> * data)
{
    QStringList clientFilter; // Strings of ids!
    QStringList projectFilter;//Yes these must be strings of ids
    QString report = "<h1>Balance report for";

    if (flags & Report::ALL_DATES)
    {
        report.append(" all dates ");
    }

    if (flags & Report::PERIOD)
    {
        qDebug(QString::number(Report::PERIOD).toUtf8());
        if ( data->keys().contains("from_date") && data->keys().contains("to_date") )
        {
            report += " for period from ";
            report += data->value("from_date").toString() + " to " + data->value("to_date").toString();
        }
        else
        {
            throw new ReportException("Invalid period data was submitted for the report");
        }
    }

    if (flags & Report::PER_CLIENT)
    {
        ClientModel* client = new ClientModel();

        report.append(" per client</h1>");

        if ( flags & Report::FILTERED)
        {
            if ( data->keys().contains("client_filter") && data->keys().contains("project_filter") )
            {
                clientFilter = data->value("client_filter").toStringList();
                projectFilter = data->value("project_filter").toStringList();
            }
            else
            {
                throw new ReportException("Invalid filter data was submitted for the report");
            }
        }

        report.append("<table border='0' width='100%'><tr bgcolor='#eee'><th>No.</th><th>Client Name</th><th>Charges</th><th>Payments</th><th>Outstanding Balance</th></tr>");

        double totalCosts = 0;
        double totalPayments = 0;

        for ( int i = 0; i < client->rowCount(); i ++)
        {
            if ( clientFilter.count() > 0 && !clientFilter.contains(client->record(i).value("id").toString()))
                continue;

            int client_id = client->record(i).value("id").toInt();
            if (client->record(i).value("client_name") == "N/A") continue;
            double cost = this->getTotalClientCost(client_id);
            totalCosts += cost;
            double payments = this->getTotalClientPayments(client_id);
            totalPayments += payments;
            report.append("<tr>")
                    .append("<td>").append(QString::number(i)).append("</td>")
                    .append("<td>").append(client->record(i).value("client_name").toString()).append("</td>")
                    .append("<td>").append(QString::number(cost)).append("</td>")
                    .append("<td>").append(QString::number(payments)).append("</td>")
                    .append("<td>").append(QString::number(cost-payments)).append("</td>")
                    .append("</tr>");

        }

        report.append("<tr>")
                .append("<td><b>").append("*").append("</b></td>")
                .append("<td><b>").append("Total").append("</b></td>")
                .append("<td><b>").append(QString::number(totalCosts)).append("</b></td>")
                .append("<td><b>").append(QString::number(totalPayments)).append("</b></td>")
                .append("<td><b>").append(QString::number(totalCosts-totalPayments)).append("</b></td>")
                .append("</tr>");

        report.append("</table>");

        delete client;
    }

    return report;
}


