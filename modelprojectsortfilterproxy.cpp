#include "projectsortfilterproxymodel.h"


ModelProjectSortFilterProxy::ModelProjectSortFilterProxy(ModelProjectProxy* model, QObject * parent) : QSortFilterProxyModel(parent)
{ 
    this->setSourceModel( model );
    this->setFilterKeyColumn(-1);
    this->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

ModelProjectSortFilterProxy::~ModelProjectSortFilterProxy()
{
}
