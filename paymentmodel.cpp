#include "paymentmodel.h"
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include "dbexception.h"
#include <QDateTime>
#include <QSqlResult>

PaymentModel::PaymentModel(QObject* parent) : Model(parent)
{
    this->tableName = "payment";
    this->init();
}


void PaymentModel::setParams()
{

  this->setRelation(1,QSqlRelation("project","id", "project_name"));

  this->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
  this->setHeaderData(1, Qt::Horizontal, QObject::tr("Project"));
  this->setHeaderData(2, Qt::Horizontal, QObject::tr("Amount"));
  this->setHeaderData(3, Qt::Horizontal, QObject::tr("Date"));
  this->setHeaderData(4, Qt::Horizontal, QObject::tr("Notes"));

}




 void PaymentModel::createTable()
 {
     QSqlQuery query(database());
     bool ret = query.exec(" create table payment "
                " (id integer primary key autoincrement, "
                " project_id integer not null,  "
                " payment_amount decimal (8,2) not null, "
                " payment_date datetime not null, "
                " payment_notes text, "
                " foreign key(project_id) references project(id) on delete cascade "
                ")");

    query.exec(" create index paymentProjectIdx on payment ( project_id ) ");

     if (!ret)
     {
         qDebug(query.lastError().text().toUtf8());
         throw new DBException("Table creation failed");
     }
     else
     {
         qDebug("Created payment table");
     }

 }

 int PaymentModel::add(int projectId, float amount, QDate date, QString notes)
 {
     QSqlRecord paymentRecord = this->editableRecord;
     paymentRecord.setValue("project_id", projectId);
     paymentRecord.setValue("payment_amount", amount);
     paymentRecord.setValue("payment_date", date);
     paymentRecord.setValue("payment_notes", notes);

     return Model::add(paymentRecord);

 }

