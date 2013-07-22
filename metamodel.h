#ifndef MODELMETA_H
#define MODELMETA_H

#include "model.h"

class MetaModel : public Model
{
    Q_OBJECT

public:
    int getRevisionNumber();
    QString getUserName();
    uint getTimeStamp();
    void associateUsername(QString username);
    static MetaModel &getInstance();

private:
    void createTable();
    void setParams();
    void versionUpdate();

    MetaModel(QObject* = 0);


public slots:
    void revise();

signals:
    void revised();

};

#endif // MODELMETA_H
