#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QSqlRecord>
#include <QStringList>


class Report
{

private:
    double getTotalProjectCost(int);
    double getTotalClientCost(int);
    double getTotalClientPayments(int);
    QString projectInvoiceRow(QSqlRecord, QStringList, int, int);

public:

    enum ReportFlags
    {
        ALL_DATES=1,
        LAST_MONTH =2,
        LAST_YEAR=4,
        THIS_MONTH=8,
        THIS_YEAR=16,
        BY_MONTH=32,
        BY_YEAR=64,
        PERIOD=128,
        SINCE=256,
        PER_PROJECT=512,
        PER_CLIENT=1024,
        YEAR=2048,
        MONTH=4096,
        FILTERED=8192,
        SUBPROJECTS=32768
    };


    Report();
    QString balanceReport(int = Report::ALL_DATES, QMap<QString, QVariant>* = NULL);
    QString generateInvoice(int = Report::ALL_DATES, QMap<QString, QVariant>* = NULL);

};

#endif // REPORT_H
