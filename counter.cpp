#include "counter.h"


Counter::Counter()
{
    stopped = false;
}

Counter::~Counter()
{
}

void Counter::run()
{
    QTime now = QTime::currentTime();
    QTime alertTime = QTime::currentTime();
    now.start();
    alertTime.start();

    while (!stopped)
    {
        if ( now.elapsed() >= 60000)
        {
            now.restart();
            emit triggered();

        }

        if ( alertTime.elapsed() >= 600000)
        {
            alertTime.restart();
            emit triggeredAlert();
        }

        sleep(2);
    }
}

void Counter::stop()
{
    stopped = true;
}
