#include "model.h"
#include <QStringList>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>
#include <dbexception.h>

Model::Model(QObject* parent):QSqlRelationalTableModel(parent, DB::get())
{
}

void Model::init()
{
    if (!this->checkTable())
    {
        this->createTable();
    }
    else
    {
        this->versionUpdate();
    }
     this->setTable(this->tableName);
     this->setEditStrategy(QSqlTableModel::OnFieldChange);
     this->editableRecord = this->record();
     this->setParams();
     this->select();
}

bool Model::select()
{
    bool res = QSqlRelationalTableModel::select();
    while (res && this->canFetchMore())
    {
        this->fetchMore();
    }

    return res;

}

bool Model::checkTable()
{
    QStringList tablelist = database().tables(QSql::Tables);
    return tablelist.contains(this->tableName);
}

int Model::add(QSqlRecord record)
{
    if (!this->insertRecord(-1, record))
    {
        qDebug(this->lastError().text().toAscii());
        qDebug(this->lastError().databaseText().toAscii());
        qDebug("there was an error");
        return -1;
    }

    if (!this->submitAll())
    {
        qDebug(this->lastError().text().toAscii());
        qDebug(this->lastError().databaseText().toAscii());
        return -1;
    }

    emit updated();
    return this->query().lastInsertId().toInt();
}

void Model::removeRow(int row, const QModelIndex &parent)
{
    QSqlRelationalTableModel::removeRow(row, parent);
    emit updated();
}

void Model::setRecord(int row, const QSqlRecord &record)
{
    QSqlRelationalTableModel::setRecord(row,record);
    emit updated();
}

QSqlRecord Model::getEditableRecord()
{
    return this->editableRecord;
}

QSqlRecord Model::record(int row, int role) const
{
    if ( role == Qt::EditRole)
    {
        QSqlTableModel mod(0, DB::get());
        mod.setTable(this->tableName);
        mod.setFilter(this->filter());
        mod.select();
        QSqlRecord rec = mod.record(row);
        return rec;
    }

    return QSqlRelationalTableModel::record(row);

}

QSqlRecord Model::record()
{
    return QSqlRelationalTableModel::record();
}

QSqlRecord Model::getRecordById(int id, int role)
{
    QSqlRecord rec;
    if (role == Qt::EditRole)
    {
        QSqlTableModel mod(0, DB::get());
        mod.setTable(this->tableName);
        mod.setFilter("id = " + QString::number(id));
        mod.select();
        rec = mod.record(0);
    }
    else
    {
        QString f = this->filter();
        this->setFilter(this->tableName+".id = " + QString::number(id));
        this->select();
        rec = QSqlRelationalTableModel::record(0);
        this->setFilter(f);
        this->select();
    }


    return rec;
}
