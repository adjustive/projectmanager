#ifndef SYNC_H
#define SYNC_H

#include <QString>
#include <QObject>
#include <QtNetwork/QNetworkReply>
#include <QMutex>
#include "metamodel.h"
class Sync : public QObject
{
        Q_OBJECT

private:
        static const QString syncServer;

public slots:
        void finished(QNetworkReply*);
        void downloadFinished(QNetworkReply *reply);
        void reopenedDB();
        void download();

public:
    Sync();
    void upload();
    void syncNow();
    QMutex syncing;
    const static int UPLOADED = 1;
    const static int DOWNLOADED = 2;



signals:
    void syncStarted();
    void downloadedNew();
    void requestFailed();
    void syncSuccess(int);
    void needToDownload();
};

#endif // SYNC_H
