#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientwindow.h"
#include "counter.h"
#include "metamodel.h"
#include "settingswindow.h"
#include "paymentmodel.h"
#include "invoicemodel.h"
#include "projectproxymodel.h"
#include "reportwindow.h"
#include <QSystemTrayIcon>
#include <QMutex>
#include <QWaitCondition>
#include <QProgressDialog>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    ClientModel* client;
    ProjectModel* modelProject;
    ProjectProxyModel* projectProxy;
    QSortFilterProxyModel* project;
    PaymentModel* payment;
    InvoiceModel* invoice;

    Ui::MainWindow *ui;
    ClientWindow *clientWindow;
    SettingsWindow *settingsWindow;
    ReportWindow *reportWindow;
    void toggle();
    Counter * timeCounter;
    void initModels();
    Sync* syncd;
    QSystemTrayIcon *trayIcon;
    bool closeable;
    void closeEvent(QCloseEvent* e);
    QMutex synchronizing;
    QWaitCondition stillSync;
    QProgressDialog *syncDownloadDialog;

public slots:
    void showClientWindow();
    void showSettingsWindow();
    void showReportWindow();
    void reopenDB();
    void syncNow();
    void fClose();
    void manageTraySignal(QSystemTrayIcon::ActivationReason);
    void tenMinuteTrigger();
    void syncStarted();
    void syncSuccess();
    void syncFailed();
    void prepareForDownload();

signals:
    void readyForDownload();
    void reopenedDB();
};

#endif // MAINWINDOW_H
