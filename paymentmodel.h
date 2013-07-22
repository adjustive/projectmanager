#ifndef MODELPAYMENT_H
#define MODELPAYMENT_H

#include "model.h"

class PaymentModel : public Model
{
public:
    PaymentModel(QObject* = 0);
    int add(int, float, QDate, QString);
    void refresh();

private:
    void createTable();
    void setParams();
};

#endif // MODELPAYMENT_H
