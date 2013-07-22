#include "projectmodelitem.h"

ProjectModelItem::ProjectModelItem(const QList<QVariant> &data, ProjectModelItem *parent)
 {
     parentItem = parent;
     itemData = data;
     this->minutes = 0 ;
     this->cost = 0;
     this->fixedPrice = -1;
     if ( !this->isRoot() && !this->parent()->isRoot() )
     {
         this->setData(2, parent->data(2));
     }
 }

ProjectModelItem::~ProjectModelItem()
 {
     qDeleteAll(childItems);
 }

void ProjectModelItem::appendChild(ProjectModelItem *item)
 {
     childItems.append(item);
 }

void ProjectModelItem::removeChild(ProjectModelItem *item)
 {
     childItems.removeAt(childItems.indexOf(item));
 }

ProjectModelItem *ProjectModelItem::child(int row)
 {
     return childItems.value(row);
 }

int ProjectModelItem::childCount() const
 {
     return childItems.count();
 }

int ProjectModelItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<ProjectModelItem*>(this));

     return 0;
 }

int ProjectModelItem::columnCount() const
{
    return itemData.count();
}

QVariant ProjectModelItem::data(int column) const
 {
     return itemData.value(column);
 }

ProjectModelItem *ProjectModelItem::parent()
 {
     return parentItem;
 }

void ProjectModelItem::setData(int column, QVariant value)
{
    this->itemData.replace(column, value);
}


void ProjectModelItem::refreshCost()
{

    this->cost = 0;
    for (int i = 0; i < this->childCount(); i++)
    {
        this->cost += this->child(i)->cost;
    }

    this->cost += this->realCost;

    if ( this->fixedPrice == -1)
    {
        this->setData(4, this->cost);
    }
    else
    {
        this->setData(4, QString::number(this->fixedPrice).append(" (").append(QString::number(this->cost)).append(")"));
        this->cost = this->fixedPrice;
    }

    if (!parent()->isRoot()) parent()->refreshCost();

}

void ProjectModelItem::addCost(double cost)
{


        this->cost += cost;
        this->setData(4, this->cost);
        if (!this->parent()->isRoot())
            this->parent()->addCost(cost);

}

void ProjectModelItem::addMinutes(int minutes)
{
    this->minutes += minutes;
    this->setData(3, QString::number((int)(this->minutes/60)).append(":").append(QString::number(this->minutes%60)));
    if ( !this->parent()->isRoot() )
        this->parent()->addMinutes(minutes);

}

/*
void ModelItemProject::setCost(double cost)
{
    double dcost = cost - this->cost;
    this->cost = cost;
    this->setData(4, this->cost);
    if (!this->parent()->isRoot())
        this->parent()->addCost(dcost);
}


void ModelItemProject::setMinutes(int minutes)
{
    int dminutes = minutes - this->minutes;
    this->minutes = minutes;
    this->setData(3, QString::number((int)(this->minutes/60)).append(":").append(QString::number(this->minutes%60)));
    if ( !this->parent()->isRoot() )
        this->parent()->addMinutes(dminutes);

}

*/

void ProjectModelItem::setParent(ProjectModelItem* newparent)
{
    this->parentItem = newparent;
}

ProjectModelItem* ProjectModelItem::find(int id)
{
    if (this->data(1).toInt() == id)
    {
        return this;
    }
    else
    {
        foreach (ProjectModelItem * item, this->childItems)
        {
            ProjectModelItem* result;
            if ( (result = item->find(id)) != NULL )
            {
                return result;
            }

        }
        return NULL;
    }
}

void ProjectModelItem::setChildrenClientName()
{
    foreach (ProjectModelItem * item , this->childItems )
    {
        item->setData(2, this->data(2));
        item->setChildrenClientName();
    }
}
