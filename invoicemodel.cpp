#include "invoicemodel.h"
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include "dbexception.h"
#include <QDateTime>
#include <QSqlResult>
#include <QTextImageFormat>
#include "projectmodel.h"
#include "settings.h"
#include "clientmodel.h"

InvoiceModel::InvoiceModel(QObject* parent) : Model(parent)
{
    this->tableName = "invoice";
    this->init();
}


void InvoiceModel::setParams()
{

  this->setRelation(1,QSqlRelation("project","id", "project_name"));

  this->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
  this->setHeaderData(1, Qt::Horizontal, QObject::tr("Project"));
  this->setHeaderData(2, Qt::Horizontal, QObject::tr("Invoice Code"));
  this->setHeaderData(3, Qt::Horizontal, QObject::tr("Amount"));
  this->setHeaderData(4, Qt::Horizontal, QObject::tr("Creation Date"));
  this->setHeaderData(5, Qt::Horizontal, QObject::tr("Notes"));
  this->setHeaderData(6, Qt::Horizontal, QObject::tr("Paid"));
  this->setHeaderData(7, Qt::Horizontal, QObject::tr("Payment Date"));


}




 void InvoiceModel::createTable()
 {
     QSqlQuery query(database());
     bool ret = query.exec(" create table invoice "
                " (id integer primary key autoincrement, "
                           " project_id integer not null,  "
                           " code varchar(12) not null,  "
                " amount decimal (8,2) not null, "
                " create_date datetime not null, "
                " notes text, "
                " paid boolean, "
                " payment_date datetime not null, "
                " foreign key(project_id) references project(id) on delete cascade "
                ")");

    query.exec(" create index invoice_project_idx on invoice( project_id ) ");

     if (!ret)
     {
         qDebug(query.lastError().text().toUtf8());
         throw new DBException("Table creation failed");
     }
     else
     {
         qDebug("Created invoice table");
     }

 }

 int InvoiceModel::add(int project_id, QString code, float amount, QDate create_date, QString notes, bool paid, QDate payment_date)
 {
     QSqlRecord invoiceRecord = this->editableRecord;
     invoiceRecord.setValue("project_id", project_id);
     invoiceRecord.setValue("code", code);
     invoiceRecord.setValue("amount", amount);
     invoiceRecord.setValue("create_date", create_date);
     invoiceRecord.setValue("notes", notes);
     invoiceRecord.setValue("paid", paid);
     invoiceRecord.setValue("payment_date", payment_date);

     return Model::add(invoiceRecord);

 }

 int InvoiceModel::countYear()
 {
     QDate date = QDate::currentDate();
     date.setDate(date.year(), 1, 1);
     QDate date2 = QDate(date.year()+1,1,1);
     InvoiceModel invoice;
     QSqlQuery query(invoice.database());
     query.prepare(" select count(*) from invoice where create_date >= :date1 and create_date <= :date2 ");
     query.bindValue(":date1", date);
     query.bindValue(":date2", date2);
     if (query.exec())
     {
         if (query.next())
         {
             return query.record().value(0).toInt();
         }
     }

     return 0;

 }

 double InvoiceModel::getTotalProjectPayments(int id)
 {

     QSqlQuery query(DB::get());

     QString q = QString("select SUM(inv.amount)")
             .append(" from invoice inv ")
             .append(" where inv.project_id = :id and inv.paid = :paid ")
             ;

     query.prepare(q);
     query.bindValue(":id",id);
     query.bindValue(":paid",true);

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

         ProjectModel *project = new ProjectModel();
         project->setFilter("parent_id = " + QString::number(id));
         project->select();

         for ( int i = 0; i < project->rowCount(); i ++)
         {
             total += InvoiceModel::getTotalProjectPayments(project->record(i).value("id").toInt());
         }

         delete project;
     }

     return total;
 }

 QString InvoiceModel::print(int row)
 {
     Settings set;
     QSqlRecord invoice = this->record(row, Qt::EditRole);
     ProjectModel project;
     ClientModel client;
     this->currency = set.getCurrency();
     QSqlRecord record = project.getRecordById(invoice.value("project_id").toInt());
     QSqlRecord clientRecord = client.getRecordByName(record.value("client_name").toString());
     QString report = "";
     QString clarifications = "";


     QString details = "<tr><td width='50%'><b>" + set.getOrganizationName().toUpper() + "</b><br>"
             + set.getAddress().replace("\n","<br>") + "<br>"
             + "Telephone: " + set.getTelephone() + "<br>"
             + "Email: " + set.getEmail() + "<br></td>";

     report.append(details);

     if (!set.getLogoPath().isEmpty())
     {
        report.append("<td width='50%'  padding='5px'><img width='50%' height='auto' src='"+set.getLogoPath()+"' /></td>");
     }

     report.append("</tr>");

     if( !invoice.value("paid").toBool())
     {
         report.append("<tr><td colspan='2'><b>INVOICE FOR "+ record.value("client_name").toString().toUpper()+"</b></td>");
     }
     else
     {
         report.append("<tr><td colspan='2'><b>RECEIPT FOR "+ record.value("client_name").toString().toUpper()+"</b></td>");
     }

     report.append("<tr><td width='50%'  padding='5px' bgcolor='#dddddd' border-color='black' border-style='solid' border='2px'><b>"+record.value("client_name").toString().toUpper()+"</b><br>"+ clientRecord.value("client_billing").toString().replace("|","<br>") +"</td>");
     report.append("<td width='50%'  padding='5px' align='right'>Invoice Date: "+  invoice.value("create_date").toDate().toString("dd/MM/yyyy"));
     report.append("<br>Invoice Number: <b>" + invoice.value("code").toString() + "</b></td></tr>");
     report.append("<tr><td colspan='2'><hr></td></tr>");




     report.append("<tr><td colspan=2><table cellspacing='0' cellpadding='5' width='100%' border='0'><tr bgcolor='#ddd'><th>Project Name</th><th width='200'>Current Invoice Cost</th></tr>");

             double totalCosts  = 0, thisInvoice = 0;


                 totalCosts += ProjectModel::getTotalProjectCost(record.value("id").toInt());
                 thisInvoice = invoice.value("amount").toDouble();
                 report.append(this->projectInvoiceRow(record, 0, thisInvoice));
                 QString total = "<b>" + currency + " " + QString::number(thisInvoice) + "</b>";
                 if ( totalCosts != thisInvoice )
                 {
                     total += " <i>(of project total " + currency + " " + QString::number(totalCosts);

                     if ( !record.value("project_finished").toBool())
                     {
                        total += " *)</i>";
                        clarifications += "<br>* This project is ongoing. Its total cost may change in the future.";
                     }
                     else
                     {
                         total += ")</i>";
                     }

                 }

             report.append("<tr><td></td><td><hr></td></tr>")
                     .append("<tr><td align='right'><b>").append("Total:   ").append("</b></td><td align='right'>").append(total).append("</td></tr>");

             if (invoice.value("paid").toBool())
             {
                 report.append("<tr><td align='right'><b>").append("Payments:   ").append("</b></td><td align='right'><b>").append(currency + " " + QString::number(thisInvoice)).append("</b></td></tr>")
                 .append("<tr><td ></td><td><hr></td></tr>")
                         .append("<tr><td align='right'><b>").append("Paid in full on:   ").append("</b></td><td align='right'><b>").append(invoice.value("payment_date").toDate().toString("dd/MM/yyyy")).append("</b></td></tr>");

             }
             report.append("</table></td></tr>");


             if ( !invoice.value("notes").toString().isEmpty())
             {
                 report.append("<tr><td colspan='2'><hr></td></tr>");
                 report.append("<tr bgcolor='#efefef'><td colspan='2'><b>Additional Notes</b><br>"+ invoice.value("notes").toString() + "</td></tr>");
             }

             return "<table  cellpadding='5'>" + report+ "</table><i>"+ clarifications + "</i>";

 }

 QString InvoiceModel::projectInvoiceRow(QSqlRecord record, int level, double cost)
 {
     QString color = "#efefef";
     QString report = "";
     QString identation = "";
     for ( int i = 0; i < level; i ++)
         identation.append("--");

     color = "#efefef";

   //  float payments = InvoiceModel::getTotalProjectPayments(record.value("id").toInt());

     if ( level == 0 )
     {
         report.append("<tr bgcolor='"+color+"'>")
               .append("<td>").append(ProjectModel::getProjectPath(record.value("id").toInt())).append("</td>")
               .append("<td align='right'>").append(currency + " " + QString::number(cost)).append("</td>")
   //            .append("<td>").append(QString::number(payments)).append("</td>")
   //            .append("<td>").append(QString::number(cost-payments)).append("</td>")
               .append("</tr>");
    }
    else
    {
        report.append("<tr bgcolor='"+color+"'>")
              .append("<td><i>").append(identation+" Includes: ").append(record.value("project_name").toString()).append("</i></td>")
              .append("<td></td>")
      //        .append("<td></td>")
      //        .append("<td></td>")
              .append("</tr>");
    }


    ProjectModel children;
         children.setFilter("parent_id = "+ record.value("id").toString());
         if ( children.rowCount() > 0)
         {
             for (int k = 0; k < children.rowCount(); k ++ )
             {
                 //if ( projectFilter.isEmpty() || projectFilter.contains(children->record(k).value("id").toString()))
                     report.append(this->projectInvoiceRow(children.record(k),level + 1, cost));
             }
         }
     return report;
 }


 QString InvoiceModel::getFilename(int row)
 {
     QSqlRecord invoice = this->record(row, Qt::EditRole);
     ProjectModel project;
     QSqlRecord record = project.getRecordById(invoice.value("project_id").toInt());
     QString filename = record.value("client_name").toString() + "_" + record.value("project_name").toString() + "_" + invoice.value("code").toString();
     filename = filename.replace(QRegExp(" |\\\\|\\/|\\*|\\?"), "_");
     return filename;

 }



