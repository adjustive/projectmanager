#include "sync.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QFile>
#include <QString>
#include <QDir>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "qupfile.h"
#include <QtScript/QScriptEngine>
#include "metamodel.h"
#include "settings.h"
#include "db.h"

const QString Sync::syncServer = "http://projectmanager.oh-mypixel.com/";

Sync::Sync()
{
}

void Sync::syncNow()
{
    if (!syncing.tryLock())
        return;

    emit syncStarted();
    Settings* set = new Settings();

    QString username = set->getUsername();
    QString password = set->getPassword();

    QUrl url(this->syncServer + "request.php");

    QNetworkRequest request(url);

    request.setRawHeader("Host", url.encodedHost());
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Accept-Language", "ru,en-us;q=0.7,en;q=0.3");
//    request.setRawHeader("Accept-Encoding", "gzip,deflate");
    request.setRawHeader("Accept-Charset", "windows-1251,utf-8;q=0.7,*;q=0.7");
    request.setRawHeader("Keep-Alive", "300");
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Referer", url.toString().toAscii());
    request.setRawHeader("Cookie", "auth=NO");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QString form = QString("username=%1&password=%2").arg(username).arg(password);
    QByteArray formencoded = QUrl::toPercentEncoding(form, "&=", "");

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    manager->post(request, formencoded);
    qDebug("Requesting Sync data");
    delete set;

}


void Sync::finished(QNetworkReply * reply)
{
    QString result;
    result = reply->readAll();
    if (reply->error() || result.isEmpty() || !result.contains(":"))
    {
        emit requestFailed();
        qDebug("Request failed");
        syncing.unlock();
        return;
    }

    qDebug(QString("Sync response: ").append(result).toAscii());


    int revisionNumber = result.split(":").at(0).toInt();
    uint timestamp = result.split(":").at(1).toUInt();
    QString username = result.split(":").at(2).toAscii();
    qDebug("Got Sync data");



    qDebug(MetaModel::getInstance().getUserName().append(" got from the db").toAscii());


    if (revisionNumber == 0)
    {
        if (MetaModel::getInstance().getUserName().isEmpty() || MetaModel::getInstance().getUserName().isNull() )
        {
            MetaModel::getInstance().associateUsername(username);
        }
    }

    if ( MetaModel::getInstance().getUserName() != username )
    {
        qDebug(MetaModel::getInstance().getUserName().append(" while received username is ").append(username).toAscii());

        emit needToDownload();//        this->download();
    }
    else
    {
        if ( revisionNumber > MetaModel::getInstance().getRevisionNumber())
        {
            qDebug(QString::number(MetaModel::getInstance().getRevisionNumber()).toAscii());
            emit needToDownload();//        this->download();
        }
        else
        {
            this->upload();
        }
    }

    reply->deleteLater();

}

void Sync::upload()
{


    Settings* set = new Settings();


       QString username = set->getUsername();
       QString password = set->getPassword();
       int revisionNumber = MetaModel::getInstance().getRevisionNumber();
       uint timestamp = MetaModel::getInstance().getTimeStamp();
    delete set;

    QString filename;

    filename = DB::getFile();

    QByteArray boundaryRegular(QString("--"+QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--"+boundaryRegular+"\r\n");
    QByteArray boundaryLast("\r\n--"+boundaryRegular+"--\r\n");

    QUrl siteurl = this->syncServer;
    QUrl url(siteurl.toString().append("/put.php"));
    QNetworkRequest request(url);
    request.setRawHeader("Host", url.encodedHost());
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Accept-Language", "en-us;q=0.7,en;q=0.3");
    request.setRawHeader("Accept-Encoding", "gzip,deflate");
    request.setRawHeader("Accept-Charset", "windows-1251,utf-8;q=0.7,*;q=0.7");
    request.setRawHeader("Keep-Alive", "300");
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Referer", "http://projectmanager.oh-mypixel.com/");
    request.setRawHeader("Content-Type", QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata1("--"+boundaryRegular+"\r\n");
    mimedata1.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    mimedata1.append("file_upload");
    mimedata1.append(boundary);
    mimedata1.append("Content-Disposition: form-data; name=\"file\"; filename=\""+filename.toUtf8()+"\"\r\n");
    mimedata1.append("Content-Type: application/octet-stream\r\n\r\n");

    QByteArray mimedata2(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"password\"\r\n\r\n");
    mimedata2.append(password.toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"revisionNumber\"\r\n\r\n");
    mimedata2.append(QString::number(revisionNumber).toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"timestamp\"\r\n\r\n");
    mimedata2.append(QString::number(timestamp).toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"username\"\r\n\r\n");
    mimedata2.append(username.toUtf8());
    mimedata2.append(boundaryLast);


    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QUpFile *upf = new QUpFile(filename, mimedata1, mimedata2);
    if (upf->openFile())
    {
        qDebug("Uploading");
 //       connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
        manager->post(request, upf);
    }

    emit syncSuccess(Sync::UPLOADED);
    syncing.unlock();
}


void Sync::download()
{

    Settings* set = new Settings();

       QString username = set->getUsername();
       QString password = set->getPassword();
    delete set;

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    QNetworkRequest request(this->syncServer + "get.php?username=" + username + "&password=" + password);
    manager->get(request);
    qDebug("Downloading");

}



void Sync::downloadFinished(QNetworkReply *reply)
{
    QString filename;
    filename = DB::getFile();

  //  QUrl url = reply->url();
    if (reply->error()) {
        qDebug("Download failed");

    } else {

        QFile file(filename);
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug("Could not open for writing");
            }

            file.write(reply->readAll());
            file.close();

            qDebug("Download succeded");
    }

    reply->deleteLater();
    emit downloadedNew();
}

void Sync::reopenedDB()
{
    emit syncSuccess(Sync::DOWNLOADED);
    syncing.unlock();

}
