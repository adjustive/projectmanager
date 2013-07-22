#ifndef MODELITEMPROJECT_H
#define MODELITEMPROJECT_H

#include <QList>
#include <QVariant>
#include <QModelIndex>

class ProjectModelItem
{
 public:
     ProjectModelItem(const QList<QVariant> &data, ProjectModelItem *parent = 0);
     ~ProjectModelItem();

     void appendChild(ProjectModelItem *child);
     void removeChild(ProjectModelItem *item);



     ProjectModelItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     int row() const;
     ProjectModelItem *parent();
     void setData(int, QVariant);
     void addMinutes(int minutes);
   //  void setMinutes(int minutes);
     bool isRoot(){ return (parentItem == 0);}
     void addCost(double cost);
  //   void setCost(double cost);
     ProjectModelItem* find(int id);
     void setParent(ProjectModelItem*);
     int minutes;
     int realMinutes;
     double realCost;
     double cost;
     double fixedPrice;
     QString totalTime;
     void setChildrenClientName();

     void refreshCost();

 private:
     QList<ProjectModelItem*> childItems;
     QList<QVariant> itemData;
     ProjectModelItem *parentItem;


 };

#endif // MODELITEMPROJECT_H
