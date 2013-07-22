#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <QString>

class Exception
{
protected:
    int error;
    QString text;
public:
    Exception(QString, int error = 0);
    QString getText() {return text; }
    int getError() { return error; }
};

#endif // EXCEPTION_H
