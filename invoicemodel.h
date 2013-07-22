#ifndef INVOICEMODEL_H
#define INVOICEMODEL_H

#include "model.h"

class InvoiceModel : public Model
{
public:
    InvoiceModel(QObject* = 0);
    int add(int project_id);
    int add(int project_id, QString code, float amount, QDate create_date, QString notes, bool paid, QDate payment_date);
    static double getTotalProjectPayments(int project_id);
    static int countYear();
    QString print(int);
    QString getFilename(int row);

private:
    QString currency;
    QString projectInvoiceRow(QSqlRecord record, int level, double cost);

    void createTable();
    void setParams();
};

#endif // INVOICEMODEL_H
