#include "settings.h"
#include <QFile>
#include <QDir>


Settings::Settings()
{

#ifdef Q_OS_LINUX
    path = QString(QDir::home().path());
    path.append(QDir::separator()).append(".ProjectManager");
    QDir *instance = new QDir();
    if (!instance->exists(path))
    {
        instance->mkdir(path);
    }
    path.append(QDir::separator()).append("settings.cfg");
    path = QDir::toNativeSeparators(path);
#else
    path = QString("settings.cfg");
#endif

    check();
    load();
}

void Settings::setDefaults()
{
    this->synchronize = false;
    this->closeToTray = false;
    this->projectsExpanded = true;
    this->showNotifications = true;
}


void Settings::check()
{

    if (QFile::exists(path))
        return;

    // file does not exist so store default values
    this->setDefaults();
    this->store();

}


void Settings::store()
{
    QFile* file;
    file = new QFile(path);
    file->open( QIODevice::WriteOnly );


    file->write((char*) &closeToTray, sizeof(bool));
    file->write((char*) &projectsExpanded, sizeof(bool));
    file->write((char*) &showNotifications, sizeof(bool));
    file->write((char*) &synchronize, sizeof(bool));
    file->write( username.toStdString().c_str(), 50);
    file->write( password.toStdString().c_str(), 50);
    file->write( organizationName.toStdString().c_str(), 50);
    file->write( address.toStdString().c_str(), 50);
    file->write( email.toStdString().c_str(), 50);
    file->write( telephone.toStdString().c_str(), 50);
    file->write( logoPath.toStdString().c_str(), 100);
    file->write( currency.toStdString().c_str(), 50);

    file->close();
    delete file;

}

void Settings::load()
{


    QFile* file;
    file = new QFile(path);
    file->open( QIODevice::ReadOnly );

    char user[50], pass[50], org[50], add[50], eml[50], tel[50], path[100], cur[50];


    file->read((char*) &closeToTray, sizeof(bool));
    file->read((char*) &projectsExpanded, sizeof(bool));
    file->read((char*) &showNotifications, sizeof(bool));
    file->read((char*) &synchronize, sizeof(bool));
    file->read( user, 50);
    file->read( pass, 50);
    file->read( org, 50);
    file->read( add, 50);
    file->read( eml, 50);
    file->read( tel, 50);
    file->read( path, 100);
    file->read( cur, 50);



    username = QString(user);
    password = QString(pass);
    organizationName = QString(org);
    address = QString(add);
    email = QString(eml);
    telephone = QString(tel);
    logoPath = QString(path);
    currency = QString(cur);

    file->close();
    delete file;
}
