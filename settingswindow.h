#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "settings.h"
#include <QtNetwork/QNetworkReply>
#include "sync.h"

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(Sync*, QWidget *parent = 0);
    ~SettingsWindow();

private:
    Settings* settings;
    Ui::SettingsWindow *ui;
    void showSettings();
    Sync* syncd;

signals:
    void settingsUpdated();

public slots:
    void saveSettings();
    void syncNow();
    void associateDB();
    void syncFailed();
    void syncStarted();
    void syncSuccess(int status);
    void getLogoPath();

};

#endif // SETTINGSWINDOW_H
