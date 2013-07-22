#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "sync.h"
#include "metamodel.h"
#include <QMessageBox>
#include <QMovie>
#include <QFileDialog>

SettingsWindow::SettingsWindow(Sync* sync, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    settings = new Settings();
    showSettings();

    this->syncd = sync;

    connect(ui->username, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->password, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->nameOrganizationLineEdit, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->addressTextEdit, SIGNAL(textChanged()), this, SLOT(saveSettings()));
    connect(ui->emailLineEdit, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->currencyLineEdit, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->telephoneLineEdit, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->logoLineEdit, SIGNAL(editingFinished()), this, SLOT(saveSettings()));
    connect(ui->expandProjectsCheckbox, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->showNotificationsCheckBox, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->closeToTrayCheckBox, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->synchronize, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
    connect(ui->syncButton, SIGNAL(clicked()), this, SLOT(syncNow()));
    connect(ui->associateAccount, SIGNAL(clicked()), this, SLOT(associateDB()));
    connect(syncd, SIGNAL(requestFailed()), this, SLOT(syncFailed()));
    connect(syncd, SIGNAL(syncStarted()), this, SLOT(syncStarted()));
    connect(syncd, SIGNAL(syncSuccess(int)), this, SLOT(syncSuccess(int)));
    connect(ui->logoBrowseButton, SIGNAL(clicked()), this, SLOT(getLogoPath()));


}

void SettingsWindow::syncFailed()
{
    if (this->isVisible())
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Sync Failed");
        msgBox.setText("Sync request failed.");
        msgBox.exec();
    }

    ui->syncButton->setDisabled(false);
    ui->associateAccount->setDisabled(false);
    ui->username->setDisabled(false);
    ui->password->setDisabled(false);

    ui->syncLabel->movie()->stop();
    delete ui->syncLabel->movie();
    ui->syncLabel->setPixmap(QPixmap(":/failed"));
}

void SettingsWindow::syncSuccess(int status)
{
    ui->syncButton->setDisabled(false);
    ui->associateAccount->setDisabled(false);
    ui->syncLabel->movie()->stop();
    delete ui->syncLabel->movie();
    ui->syncLabel->setPixmap(QPixmap(":/synced"));

    ui->username->setDisabled(false);
    ui->password->setDisabled(false);

    if (status== Sync::DOWNLOADED)
    {
        this->showSettings();
    }
}

void SettingsWindow::syncNow()
{
    syncd->syncNow();
}

void SettingsWindow::showSettings()
{
    if (!MetaModel::getInstance().getUserName().isEmpty())
    {
        ui->linkLabel->setText(QString("This database is associated with sync account <b>").append(MetaModel::getInstance().getUserName()).append("</b>"));
    }
    else
    {
        ui->linkLabel->setText("This database is not associated to a sync account.");
    }

    ui->closeToTrayCheckBox->setChecked( settings->getCloseToTray() );
    ui->expandProjectsCheckbox->setChecked( settings->getProjectsExpanded() );
    ui->showNotificationsCheckBox->setChecked( settings->getShowNotifications() );
    ui->synchronize->setChecked( settings->getSynchronize() );
    ui->username->setText( settings->getUsername() );
    ui->password->setText( settings->getPassword() );
    ui->addressTextEdit->setText(settings->getAddress());
    ui->nameOrganizationLineEdit->setText(settings->getOrganizationName());
    ui->telephoneLineEdit->setText(settings->getTelephone());
    ui->emailLineEdit->setText(settings->getEmail());
    ui->logoLineEdit->setText(settings->getLogoPath());
    ui->currencyLineEdit->setText(settings->getCurrency());

}

void SettingsWindow::saveSettings()
{
    settings->setCloseToTray(ui->closeToTrayCheckBox->checkState());
    settings->setProjectsExpanded(ui->expandProjectsCheckbox->checkState());
    settings->setShowNotifications( ui->showNotificationsCheckBox->checkState());
    settings->setSynchronize(ui->synchronize->checkState());
    settings->setUsername( ui->username->text() );
    settings->setPassword( ui->password->text() );
    settings->setAddress( ui->addressTextEdit->toPlainText() );
    settings->setOrganizationName( ui->nameOrganizationLineEdit->text() );
    settings->setTelephone(ui->telephoneLineEdit->text() );
    settings->setEmail(ui->emailLineEdit->text() );
    settings->setLogoPath( ui->logoLineEdit->text() );
    settings->setCurrency(ui->currencyLineEdit->text());

    settings->store();
    emit settingsUpdated();
}

SettingsWindow::~SettingsWindow()
{
    delete settings;
    delete ui;
}

void SettingsWindow::associateDB()
{
    MetaModel::getInstance().associateUsername(ui->username->text());
    showSettings();
    syncNow();
}

void SettingsWindow::syncStarted()
{
    ui->syncButton->setDisabled(true);
    ui->associateAccount->setDisabled(true);
    ui->username->setDisabled(true);
    ui->password->setDisabled(true);

    QMovie* synced = new QMovie(":/syncing");
    ui->syncLabel->setMovie(synced);
    ui->syncLabel->movie()->start();

}

void SettingsWindow::getLogoPath()
{
    ui->logoLineEdit->setText(QFileDialog::getOpenFileName(this, "Choose image file", QString(), "Images (*.png *.xpm *.jpg)"));
}
