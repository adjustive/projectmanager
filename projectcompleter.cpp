#include "projectcompleter.h"

ProjectCompleter::ProjectCompleter(ProjectProxyModel* project, QObject *parent) :
    QCompleter(project, parent)
{
}


QString ProjectCompleter::pathFromIndex ( const QModelIndex & index ) const
{
    QString path = "";
    if ( index.parent().isValid() )
    {
        path.append(pathFromIndex(index.parent())).append("|");
    }

    path.append(model()->data(index).toString());
    return path;
}


QStringList ProjectCompleter::splitPath ( const QString & path ) const
{
    return path.split("|");
}
