#ifndef COUNTER_H
#define COUNTER_H

#include <QThread>
#include <QTime>


class Counter : public QThread
{
    Q_OBJECT

private:

    bool stopped;
    void run();

public:
    void stop();

    Counter();
    ~Counter();

signals:
    void triggered();
    void triggeredAlert();

};

#endif // COUNTER_H
