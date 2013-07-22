#ifndef USERMODEL_H
#define USERMODEL_H

#include <QSqlTableModel>
#include <QTableView>
#include <QSqlRecord>
#include "model.h"

class ClientModel : public Model
{
public:

    ClientModel(QObject* = 0);
    void add(QString, QString billing = NULL);
    QString getName(int);
    int getId(QString);
    QString getBillingDetails(int id);
    QSqlRecord getRecordByName(QString name);

private:
    void createTable();
    void setParams();
    void versionUpdate();
};

#endif // USERMODEL_H
