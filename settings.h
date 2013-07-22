#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{

private:
    QString path;
    bool closeToTray;
    bool projectsExpanded;
    bool showNotifications;
    bool synchronize;

    QString organizationName;
    QString address;
    QString telephone;
    QString email;
    QString logoPath;
    QString currency;

    QString username;
    QString password;

    void load();
    void check();
    void setDefaults();

public:

    bool getSynchronize() { return synchronize; }
    void setSynchronize( bool synchronize ) { this->synchronize = synchronize; }

    QString getUsername() { return username; }
    void setUsername( QString username ) { this->username = username; }

    QString getPassword() { return password; }
    void setPassword( QString password ) { this->password = password; }

    QString getOrganizationName() { return organizationName; }
    void setOrganizationName( QString organizationName ) { this->organizationName = organizationName; }
    QString getAddress() { return address; }
    void setAddress( QString address ) { this->address = address; }
    QString getTelephone() { return telephone; }
    void setTelephone( QString telephone ) { this->telephone = telephone; }
    QString getEmail() { return email; }
    void setEmail( QString email ) { this->email = email; }
    QString getLogoPath() { return logoPath; }
    void setLogoPath( QString logoPath ) { this->logoPath = logoPath; }
    QString getCurrency() { return currency; }
    void setCurrency( QString currency ) { this->currency = currency; }


    bool getCloseToTray() { return closeToTray; }
    void setCloseToTray( bool closeToTray ) { this->closeToTray = closeToTray; }

    bool getProjectsExpanded() { return projectsExpanded; }
    void setProjectsExpanded( bool projectsExpanded ) { this->projectsExpanded = projectsExpanded; }

    bool getShowNotifications() { return showNotifications; }
    void setShowNotifications( bool showNotifications ) { this->showNotifications = showNotifications; }

    Settings();
    void store();

};

#endif // SETTINGS_H
