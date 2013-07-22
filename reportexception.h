#ifndef REPORTEXCEPTION_H
#define REPORTEXCEPTION_H


#include "exception.h"

class ReportException : public Exception
{
public:
    ReportException(QString , int = 0);
};

#endif // REPORTEXCEPTION_H
