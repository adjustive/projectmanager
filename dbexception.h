#ifndef DBEXCEPTION_H
#define DBEXCEPTION_H

#include <QString>
#include "exception.h"

class DBException : public Exception
{
public:
    DBException(QString, int = 0);
    ~DBException();

};


#endif // DBEXCEPTION_H
