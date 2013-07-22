#include "clientmodel.h"

#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include "dbexception.h"
#include "metamodel.h"

ClientModel::ClientModel(QObject* parent) : Model(parent)
{
    this->tableName = "client";
    this->init();
    if (this->match(this->index(0,0),Qt::DisplayRole, QVariant(1)).length() == 0)
    {
        QSqlRecord clientRecord = this->editableRecord;
        clientRecord.setValue("client_name", "N/A");
        clientRecord.setValue("client_billing", "N/A");
        clientRecord.setValue("id",1);
        Model::add(clientRecord);
    }
}


void ClientModel::setParams()
{
    //this->removeColumn(0);
    this->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    this->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    this->setHeaderData(2, Qt::Horizontal, QObject::tr("Billing Info"));
}




 void ClientModel::createTable()
 {
     bool ret = false;
     QSqlQuery query(database());
     ret = query.exec("create table client "
                      " (id integer primary key autoincrement, "
                            "client_name varchar(50) unique,"
                      "client_billing text default 'No Billing Information'"
                               ")");

     if (!ret)
     {
         throw new DBException("Table creation failed");
     }
     else
     {
         qDebug("Created client table");
     }
     this->add("N/A");
 }

 void ClientModel::add(QString name, QString billing)
 {
     QSqlRecord clientRecord = this->editableRecord;
     clientRecord.setValue("client_name", name);
     clientRecord.setValue("client_billing", billing);
     Model::add(clientRecord);
 }

 QString ClientModel::getName(int id)
 {
     qDebug(QString("Looking for id").append(QString::number(id)).toUtf8());
     QModelIndex result = this->match(this->index(0,0), Qt::EditRole, id).at(0);
     return this->data(this->index(result.row(),1)).toString();
 }

 int ClientModel::getId(QString name)
 {
     QModelIndexList results = this->match(this->index(0,1), Qt::EditRole, name);
     QModelIndex result = results.at(0);
     return this->data(this->index(result.row(),0)).toInt();

 }

 QString ClientModel::getBillingDetails(int id)
 {
     qDebug(QString("Looking for id").append(QString::number(id)).toUtf8());
     QModelIndex result = this->match(this->index(0,0), Qt::EditRole, id).at(0);
     return this->data(this->index(result.row(),2)).toString();
 }


 void ClientModel::versionUpdate()
 {
     int ver = DB::DBVER;

     if (ver < 3)
     {
         QSqlQuery query(database());
         query.exec("alter table client add column client_billing text default 'No Billing Information'");
         qDebug("Update client table to version 3");
     }
 }

 QSqlRecord ClientModel::getRecordByName(QString name)
 {
     QSqlRecord rec;

     qDebug("Looking for " + name.toUtf8());
     QString f = this->filter();
     this->setFilter(this->tableName+".client_name = '" + name +"'");
     this->select();
     rec = QSqlRelationalTableModel::record(0);
     this->setFilter(f);
     this->select();
     return rec;
 }

