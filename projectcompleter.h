#ifndef COMPLETERPROJECT_H
#define COMPLETERPROJECT_H

#include <QCompleter>
#include "projectproxymodel.h"

class ProjectCompleter : public QCompleter
{
public:
    explicit ProjectCompleter(ProjectProxyModel*, QObject* parent);
    QString pathFromIndex ( const QModelIndex & index ) const;
    QStringList splitPath ( const QString & path ) const;

};

#endif // COMPLETERPROJECT_H
