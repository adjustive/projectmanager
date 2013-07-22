#include "reportexception.h"

ReportException::ReportException(QString text, int err ) : Exception("Report Exception: " + text, err)
{
}

