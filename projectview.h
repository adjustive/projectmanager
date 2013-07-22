#ifndef VIEWPROJECT_H
#define VIEWPROJECT_H

#include <QTreeView>
#include <QDropEvent>
#include <QList>
#include <QAction>
#include <QSortFilterProxyModel>

class ProjectView : public QTreeView
{
    Q_OBJECT

private:
    QSortFilterProxyModel* modelProxy;
    QList<int>* activeProjects;
    bool showArchived;

public:
    explicit ProjectView(QWidget *parent = 0);
    void setModel(QSortFilterProxyModel*);

signals:

public slots:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void toggleArchived(bool);
    void setFilter(QString);
};

#endif // VIEWPROJECT_H
