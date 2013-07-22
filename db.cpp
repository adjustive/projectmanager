#include "db.h"

#include <QDir>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include "dbexception.h"
#include <QDir>

// DB

bool DB::initialized = false;
int DB::DBVER = 3;

void DB::destroyDB()
{
    QSqlDatabase::removeDatabase(DB::getName());
    DB::initialized = false;

}

QString DB::getName()
{
    return "MVCDB";
}

QString DB::getFile()
{
    QString filename;
    #ifdef Q_OS_LINUX
        filename = QString(QDir::home().path());
        filename.append(QDir::separator()).append(".ProjectManager");
        QDir *instance = new QDir();
        if (!instance->exists(filename))
        {
            instance->mkdir(filename);
        }
        filename.append(QDir::separator()).append("data.slit");
        filename = QDir::toNativeSeparators(filename);
    #else
        filename = QString("data.slit");
    #endif
    return filename;
}

QSqlDatabase DB::get()
{
    QSqlDatabase db;
    if ( !DB::initialized )
    {
        db = QSqlDatabase::addDatabase("QSQLITE", DB::getName());
        db.setDatabaseName(DB::getFile());
        if  ( !db.isOpen())
        {
            if (!db.open())
            {
                throw new DBException("Cannot open");
            }
        }
        QSqlQuery query(db);
        query.exec("PRAGMA foreign_keys = ON;");
        query.exec("PRAGMA ignore_check_constraints = OFF;");

        qDebug("Initializing DB");
         DB::initialized = true;
    }
    db = QSqlDatabase::database(DB::getName());
    if  ( !db.isOpen())
    {
        if (!db.open())
        {
            throw new DBException("Cannot open");
        }
    }

    return db;
}

