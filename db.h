#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QString>

#define DB_NULL -1

class DB
{
private:
    static QString getName();
    DB();
    static bool initialized;

public:
    static void destroyDB();
    static QSqlDatabase get();
    static QString getFile();
    static int DBVER;
};



#endif // DB_H
