#include "exception.h"

Exception::Exception(QString text, int err )
{
    this->error = err;
    this->text = text;
}

