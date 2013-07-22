#include "metamodel.h"
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include "dbexception.h"
#include <QDateTime>
#include <QSqlResult>

MetaModel::MetaModel(QObject* parent) : Model(parent)
{
    this->tableName = "meta";
    this->init();
}


void MetaModel::setParams()
{
}


 void MetaModel::createTable()
 {
     bool ret = false;
     QSqlQuery query(database());
     ret = query.exec("create table meta "
                      "(id integer, "
                      "revisionNumber integer, "
                      "timestamp integer, "
                      "username varchar(32),"
                      "db_version integer"
                      ")");


     if (!ret)
     {
         throw new DBException("Table creation failed");
     }
     else
     {
         qDebug("Created meta table");
         query.prepare("insert into meta values (0,  0, 0, '', ?)");
         query.bindValue(0,DB::DBVER);
         ret = query.exec();
         if ( ret )
         {
            qDebug (" and initialized it");
         }
         else
         {
             query.exec("drop table meta");
             qDebug( query.lastQuery().toUtf8() );
             qDebug( query.lastError().text().toUtf8() );
         }
     }

 }

 void MetaModel::revise()
 {

         QDateTime *t = new QDateTime();
         QSqlRecord metaRecord = this->editableRecord;
         metaRecord.setValue("id", 0);
         metaRecord.setValue("revisionNumber", this->getRevisionNumber() + 1);
         metaRecord.setValue("timestamp", t->toTime_t());
         metaRecord.setValue("username", this->getUserName());
         Model::setRecord(0, metaRecord);

         qDebug(QString("revision set to ").append(QString::number(this->getRevisionNumber())).toUtf8());
         delete t;
         emit revised();
}

 void MetaModel::associateUsername(QString username)
 {
    QSqlRecord rec = this->record(0);
    rec.setValue("username", username);
    this->setRecord(0,rec);
    revise();
 }

 QString MetaModel::getUserName()
 {
     return this->record(0).value("username").toString();
 }

 int MetaModel::getRevisionNumber()
 {
     return this->record(0).value("revisionNumber").toInt();
 }


 uint MetaModel::getTimeStamp()
 {
     return this->record(0).value("timestamp").toUInt();
 }

 void MetaModel::versionUpdate()
 {
     QSqlQuery query(database());
     bool ret = query.exec("select db_version from meta");
     int ver=0;

     if (!ret)
     {
         ver = 0;
     }
     else
     {
         if(query.next())
            ver = query.value(0).toInt();
         else
            ver = 0;
     }


     if ( ver < 1 )
     {
         query.exec("alter table meta add column db_version integer");
         query.exec("alter table meta add column username varchar(32)");
         query.exec("update meta set db_version = 1 where id = 0");

     }

     if ( ver < 2 )
     {
         query.exec("update meta set db_version = 2 where id = 0");
         qDebug("Updated meta table to version 2");
     }

     if (ver < 3 )
     {
         query.exec("update meta set db_version = 3 where id = 0");
         qDebug("Updated meta table to version 3");

     }
     ver = 3;
     DB::DBVER = ver;


 }

 MetaModel& MetaModel::getInstance()
 {
     static MetaModel instance;

     return instance;
 }
