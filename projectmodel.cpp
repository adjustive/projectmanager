#include "projectmodel.h"

#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include <QSqlRecord>
#include <QDate>
#include "dbexception.h"
#include "db.h"

ProjectModel::ProjectModel(QObject* parent) : Model(parent)
{

    this->tableName = "project";
    this->init();
    //this->setupModelData();
}


void ProjectModel::setParams()
{

    this->setRelation(1,QSqlRelation("client","id", "client_name"));

    this->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    this->setHeaderData(1, Qt::Horizontal, QObject::tr("Client"));
    this->setHeaderData(2, Qt::Horizontal, QObject::tr("Parent"));
    this->setHeaderData(3, Qt::Horizontal, QObject::tr("Minutes"));
    this->setHeaderData(4, Qt::Horizontal, QObject::tr("Rate"));
    this->setHeaderData(5, Qt::Horizontal, QObject::tr("Start"));
    this->setHeaderData(6, Qt::Horizontal, QObject::tr("End"));
    this->setHeaderData(7, Qt::Horizontal, QObject::tr("Notes"));
    this->setHeaderData(8, Qt::Horizontal, QObject::tr("Finished"));
    this->setHeaderData(9, Qt::Horizontal, QObject::tr("Archived"));
    this->setHeaderData(10, Qt::Horizontal, QObject::tr("Name"));

}




void ProjectModel::createTable()
{
    QSqlQuery query(database());
    bool ret = query.exec(" create table project "
               " (id integer primary key autoincrement, "
               " client_id integer not null,  "
               " parent_id integer default null, "
               " project_minutes integer, "
               " project_rate integer not null, "
               " project_start datetime not null, "
               " project_end datetime default null, "
               " project_notes text, "
               " project_finished boolean,"
               " project_archived boolean default 0,"
               " project_name varchar(30) not null, "
               " project_set_price decimal(10,2) default null, "
               " foreign key(parent_id) references project(id) on delete cascade, "
               " foreign key(client_id) references client(id) "
               ")");

    query.exec(" create index projectParentIdx on project ( parent_id ) ");
    query.exec(" create index projectClientIdx on project ( client_id ) ");

    if (!ret)
    {
        throw new DBException("Project table creation failed");
    }
    else
    {
        qDebug("Created project table");
    }

}

int ProjectModel::add(int clientId, int parentId, int minutes, int rate, QDate start, QDate end, QString notes, bool finished, bool archived, QString name, double set_price)
{
    QSqlRecord projectRecord = this->editableRecord;
    projectRecord.setValue("client_id", clientId);
    if (parentId != 0) projectRecord.setValue("parent_id", parentId);
    projectRecord.setValue("project_minutes", minutes);
    projectRecord.setValue("project_rate", rate);
    projectRecord.setValue("project_start", start);
    projectRecord.setValue("project_end", end);
    projectRecord.setValue("project_notes", notes);
    projectRecord.setValue("project_finished", finished);
    projectRecord.setValue("project_archived", archived);
    projectRecord.setValue("project_name", name);
    if (set_price != 0) projectRecord.setValue("project_set_price",set_price );

   return Model::add(projectRecord);

}

void ProjectModel::versionUpdate()
{
    int ver = DB::DBVER;
}



double ProjectModel::getTotalProjectCost(int id)
{
    ProjectModel* project = new ProjectModel();

    double total = 0;

    QString filter = "project.id = ";
    filter.append(QString::number(id));
    project->setFilter(filter);
    project->select();

    if ( project->rowCount() != 1 )
    {
        delete project;
        throw new DBException("Invalid id given!");
    }



    if ( !project->record(0).value("project_set_price").isNull() )
    {
        total = project->record(0).value("project_set_price").toDouble();
    }
    else
    {
        total += project->record(0).value("project_rate").toDouble() * project->record(0).value("project_minutes").toDouble() /60;
        filter = "project.parent_id = ";
        filter.append(QString::number(id));

        project->setFilter(filter);
        project->select();

        for (int i = 0; i < project->rowCount(); i ++)
        {
            total += getTotalProjectCost(project->record(i).value("id").toInt());
        }

    }

    delete project;
    return total;
}


QString ProjectModel::getProjectPath(int id)
{
    ProjectModel project;

    QString filter = "project.id = ";
    filter.append(QString::number(id));
    project.setFilter(filter);
    project.select();


    if ( project.rowCount() != 1 )
    {
        throw new DBException("Invalid id given!");
    }

    if ( project.record(0).value("parent_id").isNull() )
    {
        return project.record(0).value("project_name").toString();
    }
    else
    {
        return ProjectModel::getProjectPath(project.record(0).value("parent_id").toInt()) + " - " + project.record(0).value("project_name").toString();

    }
}
