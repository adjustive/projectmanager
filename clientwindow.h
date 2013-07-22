#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include "clientmodel.h"
namespace Ui {
    class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = 0);
    ~ClientWindow();
    void init(ClientModel*);

private:
    Ui::ClientWindow *ui;
    ClientModel* model;
    void closeEvent(QCloseEvent *);

signals:
    void closed();

   public slots:
    void addClient();
    void deleteClient();
};

#endif // CLIENTWINDOW_H
