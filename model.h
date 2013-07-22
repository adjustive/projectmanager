#ifndef MODEL_H
#define MODEL_H

#include <QSqlRelationalTableModel>
#include <QStandardItemModel>
#include <QSqlRecord>
#include "db.h"

class Model  : public QSqlRelationalTableModel
{
    Q_OBJECT
protected:
    QSqlRecord editableRecord;
    QString tableName;
    virtual void setParams() = 0;
    virtual void createTable() = 0;
    bool checkTable();
    int add(QSqlRecord);
    virtual void versionUpdate() {}
    void setTableVersion(int version);
public:
    Model(QObject*);
    void init();
    void setRecord(int row, const QSqlRecord &record);
    QSqlRecord getEditableRecord();
    void removeRow(int, const QModelIndex& = QModelIndex());
    QString showSelect() { return this->selectStatement(); }
    QSqlRecord record(int row, int role = Qt::DisplayRole) const;
    QSqlRecord record();
    QSqlRecord getRecordById(int id, int role = Qt::DisplayRole);
    bool select();
signals:
    void updated();

};

#endif // MODEL_H
