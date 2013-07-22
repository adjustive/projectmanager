#include "dbexception.h"

DBException::DBException(QString text, int err ) : Exception("Database Exception: " + text, err)
{
}
