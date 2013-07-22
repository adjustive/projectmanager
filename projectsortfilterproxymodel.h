#ifndef MODELPROJECTSORTFILTERPROXY_H
#define MODELPROJECTSORTFILTERPROXY_H

#include <QSortFilterProxyModel>
#include <modelprojectproxy.h>
class ModelProjectSortFilterProxy : public QSortFilterProxyModel
{
public:
    ModelProjectSortFilterProxy(ModelProjectProxy*, QObject* = 0);
    ~ModelProjectSortFilterProxy();

};

#endif // MODELPROJECTSORTFILTERPROXY_H
