#include "projectproxymodel.h"
#include "projectmodel.h"
#include "clientmodel.h"
#include <QSqlRecord>
#include <QDate>
#include <QSortFilterProxyModel>
#include <QSqlField>
#include <QFile>


ProjectProxyModel::ProjectProxyModel(ProjectModel* project, QObject* parent) : QAbstractItemModel(parent)
{
    this->project = project;
    QList<QVariant> rootData;
    rootData << "Name" << "ID" << "Client" << "Total Time" << "Total Cost" << "Archived";
    rootItem = new ProjectModelItem(rootData);
    this->setupModelData(rootItem);

    this->connect(project, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataUpdated(QModelIndex, QModelIndex)));
}

ProjectProxyModel::~ProjectProxyModel()
{
    delete rootItem;
}

QModelIndex ProjectProxyModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ProjectModelItem *childItem = static_cast<ProjectModelItem*>(index.internalPointer());
    ProjectModelItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex ProjectProxyModel::index(int row, int column, const QModelIndex &parent) const
{

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ProjectModelItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ProjectModelItem*>(parent.internalPointer());


    ProjectModelItem *childItem = parentItem->child(row);

    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
        return QModelIndex();
}

int ProjectProxyModel::rowCount(const QModelIndex &parent) const
{
    ProjectModelItem *parentItem;
    //if (parent.column() > 0)
    //  return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ProjectModelItem*>(parent.internalPointer());

    return parentItem->childCount();
}

bool ProjectProxyModel::hasChildren(const QModelIndex &parent) const
{
    //  qDebug("Je suis runnez");

    //  qDebug(QString("commande de la children ").append(QString::number(rowCount(parent))).toUtf8());
    return rowCount(parent) > 0;
}

int ProjectProxyModel::columnCount(const QModelIndex &parent) const
{
    //   qDebug("calling columncount");

    if (parent.isValid())
        return static_cast<ProjectModelItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}


QVariant ProjectProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ProjectModelItem *item = static_cast<ProjectModelItem*>(index.internalPointer());
    QSqlRecord record;
    record = getRecord(item->data(1).toInt());

    if (role == Qt::DecorationRole )
    {
        if ( index.column() == 0 && activeProjects.contains(item->data(1).toInt()))
        {
            QPixmap image(":/active");
            return image;
        }


        if ( index.column() == 0 && record.value("project_archived").toBool() )
        {
            QPixmap image(":/archived");
            return image;
        }

        if ( index.column() == 0 && record.value("project_finished").toBool() )
        {
            QPixmap image(":/finished");
            return image;
        }
    }

    /*  if (index.column() == 0 && role == Qt::EditRole) // this is for the completer
     {
         QString fullPath = "";
         if ( index.parent().isValid() )
         {
             fullPath.append(this->data(index.parent(),Qt::EditRole).toString()).append("|");
         }
         fullPath.append(item->data(0).toString());
         return fullPath;
     }
    */
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return item->data(index.column());
}

void ProjectProxyModel::setupModelData(ProjectModelItem *parent)
{
    // qDebug("calling setupdata");

    QVariant parentId;
    QModelIndexList results;
    if (parent == this->rootItem)
    {

        for (int i = 0; i < project->rowCount(); i ++)
        {

            if ( project->record(i).value("parent_id").isNull() )
            {
                results.append(project->index(i,0));
            }
        }
        parentId.convert(QVariant::Int);
    }
    else
    {

        parentId = parent->data(1);
        results = project->match(project->index(0,2),Qt::DisplayRole,parentId,-1, Qt::MatchExactly);

    }
    foreach (QModelIndex result, results)
    {
        QSqlRecord data = project->record(result.row());
        QList<QVariant> proxyData;
        proxyData << data.value("project_name") << data.value("id") << project->data(project->index(result.row(),1));
        int minutes = data.value("project_minutes").toInt();
        double cost;
        double realcost = (double)(data.value("project_rate").toFloat() * data.value("project_minutes").toFloat() / 60);
        if ( data.value("project_set_price").isNull() )
        {
            cost = realcost;
        }
        else
        {
            cost = data.value("project_set_price").toDouble();
        }
        cost = QString::number(cost,'f', 2).toDouble();
        proxyData << minutes << cost << data.value("project_archived").toBool();

        ProjectModelItem* newItem = new ProjectModelItem(proxyData, parent);

        if ( data.value("project_set_price").isNull() )
        {
            newItem->fixedPrice = -1;
        }
        else
        {
            newItem->fixedPrice = cost;
        }
        newItem->realCost = realcost;
        newItem->realMinutes = minutes;

        //     newItem->addCost(cost);
        newItem->addMinutes(minutes);
        parent->appendChild(newItem);
        newItem->refreshCost();

        this->setupModelData(newItem);
    }

}

QVariant ProjectProxyModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    //   qDebug("calling headerdata");

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

void ProjectProxyModel::refresh() // you must change me to make me minimally update and not rebuild the model with changes.
{
    qDebug("I should not be running");
    this->beginResetModel();
    project->select();
    delete this->rootItem;
    QList<QVariant> rootData;
    rootData << "Name" << "ID" << "Client" << "Total Time" << "Total Cost";
    rootItem = new ProjectModelItem(rootData);
    this->setupModelData(rootItem);
    this->endResetModel();

}

void ProjectProxyModel::dataUpdated(QModelIndex from, QModelIndex to)
{

    int changedId = project->data(project->index(from.row(), 0)).toInt();
    ProjectModelItem* modelItem = this->getModelItemById(changedId);

    if ( to.column() == 10) //name changed
    {
        qDebug("name changed to");
        qDebug(to.data().toByteArray());
        modelItem->setData(0, to.data().toString());
        emit variantUpdated(changedId, "name", to.data());
    }

    if (to.column() == 3 || to.column() == 4 )
    {
        if (to.column() == 3) // minutes changed
        {
            qDebug("minutes changed to");
            qDebug(QString::number(to.data().toInt()).toUtf8());
            modelItem->addMinutes( to.data().toInt() - modelItem->realMinutes );
            modelItem->realMinutes =to.data().toInt();
            modelItem->realCost = (double)(project->data(project->index(from.row(), 4)).toFloat() * modelItem->realMinutes / 60);
            emit variantUpdated(changedId, "minutes", to.data());

        }

        if (to.column() == 4)//rate changed
        {
            modelItem->realCost = (double)(to.data().toFloat() * modelItem->realMinutes / 60);
            emit variantUpdated(changedId, "rate", to.data());

        }
        modelItem->refreshCost();


    }

    if (to.column() == 11)
    {

        if ( to.data().isNull() )
        {
            modelItem->fixedPrice = -1;
        }
        else
        {
            modelItem->fixedPrice = to.data().toDouble();
        }

        modelItem->refreshCost();
        emit variantUpdated(changedId, "fixed price", to.data());

    }


    if ( to.column() == 9 )
    {
        modelItem->setData(5, to.data().toBool());
        emit variantUpdated(changedId, "finished", to.data());

    }


    if (to.column() == 2 )
    {

        if ( (modelItem->parent()->isRoot() && !to.data().isNull() ) || ( !modelItem->parent()->isRoot() && modelItem->parent()->data(1) != to.data().toInt() ) )
        {
            // qDebug(modelItem->parent()->data(1).toByteArray());

            emit layoutAboutToBeChanged();

            // changed a parent
            if ( !modelItem->parent()->isRoot() )
            {
                modelItem->parent()->addMinutes(-modelItem->minutes);
                modelItem->parent()->removeChild(modelItem);
                modelItem->parent()->refreshCost();
            }
            else
            {
                modelItem->parent()->removeChild(modelItem);
            }

            if (!to.data().isNull())
            {
                modelItem->setParent(this->getModelItemById(to.data().toInt()));
                modelItem->parent()->addMinutes(modelItem->minutes);
                modelItem->parent()->addCost(modelItem->cost);
                modelItem->parent()->appendChild(modelItem);
                modelItem->parent()->setChildrenClientName();

            }
            else
            {
                modelItem->setParent(this->rootItem);
                modelItem->parent()->appendChild(modelItem);
            }

            emit layoutChanged();
            modelItem->refreshCost();

        }

        emit variantUpdated(changedId, "parent", to.data());

    }
    if (to.column() == 1)
    {
        ClientModel client;
        int client_id = client.getId(to.data().toString());
     //   ProjectModelItem* modelItem = this->getModelItemById(changedId);
        for( int i = 0; i < modelItem->childCount(); i ++)
        {
            int childId = modelItem->child(i)->data(1).toInt();
            this->setValue(childId,"client_id", client_id);
        }
        modelItem->setData(2, to.data().toString());

        emit variantUpdated(changedId, "client", to.data());
    }


}

void ProjectProxyModel::add(int clientId, int parentId, int minutes, int rate, QDate start, QDate end, QString notes, bool finished, bool archived, QString name, double set_price)
{
    int id = project->add(clientId, parentId, minutes, rate, start,end, notes, finished, archived, name, set_price);


    if ( id == -1 )
        return;

    emit layoutAboutToBeChanged();

    ProjectModelItem* parent;

    if (parentId == 0)
    {
        parent = this->rootItem;
    }
    else
    {
        parent = this->getModelItemById(parentId);
    }


    QModelIndexList matches = project->match(project->index(0,0),Qt::DisplayRole, id);

    matches = project->match(project->index(0,0),Qt::DisplayRole, id);

    QModelIndex newProject = matches.at(0);



    QSqlRecord data = project->record(newProject.row());
    QList<QVariant> proxyData;
    proxyData << data.value("project_name") << data.value("id") << project->data(project->index(newProject.row(),1));
    minutes = data.value("project_minutes").toInt();
    double cost = (double)(data.value("project_rate").toFloat() * data.value("project_minutes").toFloat() / 60);
    cost = QString::number(cost,'f', 2).toDouble();
    proxyData << minutes << cost << data.value("project_archived").toBool();

    ProjectModelItem* newItem = new ProjectModelItem(proxyData, parent);
    newItem->realCost = cost;
    newItem->realMinutes = minutes;
    parent->appendChild(newItem);
    emit layoutChanged();
}


Qt::ItemFlags ProjectProxyModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);


    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;


}

bool ProjectProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{



    if (action == Qt::IgnoreAction)
        return true;

    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    int rows = 0;

    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        newItems << text;
        ++rows;
    }
    QVariant parentId;
    parentId.convert(QVariant::Int);
    if (parent.isValid())
    {
        ProjectModelItem* par = static_cast<ProjectModelItem*>(parent.internalPointer());
        parentId = par->data(1);
    }

    int thisId = newItems.at(0).toInt();

    if ( thisId == parentId )
        return false;

    // QModelIndex parent = project->match(project->index(0,0),Qt::DisplayRole, parentId)[0];
    QModelIndex item = project->match(project->index(0,0),Qt::EditRole, thisId)[0];


    QSqlRecord rec = project->record(item.row());
    if ( parentId.isNull())
    {
        rec.setNull("parent_id");
    }
    else
    {
        rec.setValue("parent_id", parentId);
    }
    //  return true;


    if (parentId.isNull())
    {
        qDebug("This is a null value!");
    }

    project->setRecord(item.row(), rec);
    return true;
}

bool ProjectProxyModel::supportedDropActions()
{
    return Qt::MoveAction;
}

QStringList ProjectProxyModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

QMimeData *ProjectProxyModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            ProjectModelItem* item = static_cast<ProjectModelItem*>(index.internalPointer());
            QString text = item->data(1).toString();
            stream << text;
        }
    }

    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool ProjectProxyModel::setValue(int recordId, QString columnName, const QVariant &value)
{
    int row = this->getRecordRow(recordId);

    QSqlRecord record = project->record(row);

    if (columnName == "client_id")
    {
        record.append(QSqlField("client_id", QVariant::Int));
        record.setValue("client_id", value);
    }
    else
    {
        if (record.value(columnName)==value && (record.value(columnName).isNull() == value.isNull() ))
            return false;
    }

    if ( value.isNull() )
    {
        record.setNull(columnName);
    }
    else
    {
        record.setValue(columnName,value);
    }
    project->setRecord(row, record);
    return true;

}

/*!
  Gets the record row from the project not the proxy.
  */
int ProjectProxyModel::getRecordRow(QModelIndex index) const
{

    ProjectModelItem *item = static_cast<ProjectModelItem*>(index.internalPointer());
    int itemId = item->data(1).toInt();

    // qDebug(QString::number(itemId).toUtf8());
    QModelIndex result = project->match(project->index(0,0), Qt::DisplayRole, itemId).at(0);
    return result.row();
}

/*!
  Gets the record row from the project not the proxy.
  */
int ProjectProxyModel::getRecordRow(int recordId) const
{
    QModelIndex result = project->match(project->index(0,0), Qt::EditRole, recordId).at(0);
    return result.row();
}

/*!
  Gets the record from the project not the proxy. The index is of the proxy
  */
QSqlRecord ProjectProxyModel::getRecord(QModelIndex index, int role) const
{
    return project->record(getRecordRow(index), role);
}

/*!
  Gets the record from the project not the proxy.
  */
QSqlRecord ProjectProxyModel::getRecord(int recordId, int role) const
{
    return project->record(this->getRecordRow(recordId), role);
}

void ProjectProxyModel::addNewProject()
{
    this->add(1,0,0,25,QDate::currentDate(), QDate::currentDate(), "Enter notes here", false, false, "New Project", 0);
}

void ProjectProxyModel::deleteProject(int id)
{
    emit layoutAboutToBeChanged();
    ProjectModelItem* toDelete = this->getModelItemById(id);
    int row = this->getRecordRow(id);
    project->removeRow(row);
    toDelete->parent()->removeChild(toDelete);
    emit layoutChanged();
}

ProjectModelItem* ProjectProxyModel::getModelItemById(int id)
{
    return this->rootItem->find(id);
}

void ProjectProxyModel::minuteTrigger()
{
    foreach ( int i, activeProjects)
    {
        this->setValue(i,"project_minutes", this->getRecord(i).value("project_minutes").toInt()+1);
    }
}

void ProjectProxyModel::activateProject(int id)
{
    activeProjects.append(id);
}


void ProjectProxyModel::deactivateProject(int id)
{
    activeProjects.removeAt(activeProjects.indexOf(id));
}

QList<int>& ProjectProxyModel::getActiveProjects()
{
    return activeProjects;
}

QList<int>* ProjectProxyModel::getDescendants(int id, ProjectModelItem *item)
{
    QList<int>* list= new QList<int>();
    list->append(id);

    if (item == NULL)
    {
        item = rootItem->find(id);
    }

    for( int i = 0; i < item->childCount(); i ++ )
    {
        QList<int>* childlist = this->getDescendants(item->child(i)->data(1).toInt(), item->child(i));

        list->append(*childlist);

        delete childlist;
    }

    return list;

}

