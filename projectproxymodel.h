#ifndef MODELPROJECTPROXY_H
#define MODELPROJECTPROXY_H

#include <QSortFilterProxyModel>
#include <QMimeData>
#include "projectmodelitem.h"
#include "projectmodel.h"
#include <QSet>

class ProjectProxyModel  : public QAbstractItemModel
{
    Q_OBJECT

private:
    void setupModelData(ProjectModelItem *parent);
    ProjectModelItem* rootItem;
    ProjectModel* project;
    ProjectModel* getModel() { return project; }
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    QList<int> activeProjects;


public:
    ProjectProxyModel(ProjectModel*, QObject* = 0);
    ~ProjectProxyModel();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;

    QList<int>& getActiveProjects();

    QVariant data(const QModelIndex &index, int role =  Qt::DisplayRole) const;

    QModelIndex parent(const QModelIndex &child = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation,int role) const;
    void add(int clientId, int parentId, int minutes, int rate, QDate start, QDate end, QString notes, bool finished, bool archived, QString name, double set_price);
    void deleteProject(int);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    bool supportedDropActions();
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

    bool setValue(int recordId, QString columnName, const QVariant &value);
    int getRecordRow(int) const;
    int getRecordRow(QModelIndex) const;
    QSqlRecord getRecord(QModelIndex, int role = Qt::DisplayRole) const;
    QSqlRecord getRecord(int recordId, int role = Qt::DisplayRole) const;
    ProjectModelItem* getModelItemById(int id);

    void deactivateProject(int);
    void activateProject(int);

    QList<int>* getDescendants(int, ProjectModelItem* = NULL);


public slots:
    void refresh();
    void addNewProject();
    void dataUpdated(QModelIndex, QModelIndex);
    void minuteTrigger();

signals:
    void variantUpdated(int, QString, QVariant);


};

#endif // MODELPROJECTPROXY_H
