#include "projectview.h"

#include <QMenu>


ProjectView::ProjectView(QWidget *parent) :
    QTreeView(parent)
{

    this->setSortingEnabled(true);
    this->setDragDropMode(QTreeView::DragDrop);
    this->setDragEnabled(true);
    this->setDropIndicatorShown(true);
    this->viewport()->setAcceptDrops(true);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void ProjectView::setModel(QSortFilterProxyModel *project)
{
   modelProxy = project;
   QTreeView::setModel(project);

   this->showArchived = false;
   toggleArchived(this->showArchived);

   this->hideColumn(1);
   this->hideColumn(5);
   this->resizeColumnToContents(0);
   this->setFocus();
}

void ProjectView::toggleArchived(bool check)
{
    QModelIndexList archived = modelProxy->match(modelProxy->index(0,5),Qt::DisplayRole,QVariant(true), -1, Qt::MatchRecursive);
    foreach (QModelIndex arch, archived)
    {
        this->setRowHidden(arch.row(), arch.parent(), !check);
    }
    this->showArchived = check;
}


void ProjectView::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeView::dragEnterEvent(event);
 //   qDebug("Dragging Entered..");
    event->accept();
}

void ProjectView::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeView::dragMoveEvent(event);
//    qDebug("Drag Move Event...");
    event->acceptProposedAction();
}

void ProjectView::dropEvent(QDropEvent *event)
{
    QTreeView::dropEvent(event);
 //   qDebug("Drop Event... NOP");
    event->accept();
}

void ProjectView::setFilter(QString filter)
{
    modelProxy->setFilterFixedString(filter);
    this->toggleArchived(this->showArchived);
}

