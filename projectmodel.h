#ifndef MODELPROJECT_H
#define MODELPROJECT_H

#include "model.h"
#include <QModelIndex>

class ProjectModel : public Model
{
public:

    ProjectModel(QObject* = 0);
    int add(int, int, int, int, QDate, QDate, QString, bool, bool, QString, double);
    void refresh();
    static double getTotalProjectCost(int id);
    static QString getProjectPath(int id);

private:
    void createTable();
    void setParams();
    void versionUpdate();
};

#endif // MODELPROJECT_H
