#include "dbmanager.h"

DBManager::DBManager() {
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("D:/soft/Qt/projects/ADog/ADog.db");
    if (!database.open())
    {
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

bool DBManager::createItem(const QString &appName)
{

    QSqlQuery query;
    query.prepare("INSERT INTO AppUsage (app_name, usage_date, usage_time) "
                  "VALUES (:app_name, :usage_date, :usage_time)");
    query.bindValue(":app_name", appName);
    query.bindValue(":usage_date", "2024-7-17");
    query.bindValue(":usage_time", "1500");
    if(query.exec()){
        return true;
    }
    else{
        qDebug() << "create fails";
        return false;
    }
}

bool DBManager::updateItem(const QString &appName, const QString &date, const QString &usageTime)
{
    QSqlQuery query;
    query.prepare("UPDATE app_usage SET usage_date = :usage_date, usage_time = :usage_time "
                  "WHERE app_name = :app_name");
    query.bindValue(":usage_date", date);
    query.bindValue(":usage_time", usageTime);
    query.bindValue(":app_name", appName);
    if(query.exec()){
        return true;
    }
    else{
        qDebug() << "create fails";
        return false;
    }

}

void DBManager::readByDate(const QString &date, QMap<QString, QString> &res)
{
    QSqlQuery query;
    query.prepare("SELECT app_name, usage_time "
                  "FROM AppUsage WHERE usage_date = :date");
    query.bindValue(":date", date);
    if (!query.exec()) {
        qDebug() << "Query execution error: " << query.lastError().text();
        return;
    }

    // 读取查询结果并存储到res中
    while (query.next()) {
        QString appName = query.value(0).toString();
        QString time = query.value(1).toString();
        res.insert(appName, time);
    }
}

void DBManager::readByAppName(const QString &appName, QMap<QString, QString> &res)
{
    QSqlQuery query;
    query.prepare("SELECT usage_date, usage_time "
                  "FROM AppUsage WHERE app_name = :appName");
    query.bindValue(":appName", appName);
    if (!query.exec()) {
        qDebug() << "Query execution error: " << query.lastError().text();
        return;
    }

    // 读取查询结果并存储到res中
    while (query.next()) {
        QString usage_date = query.value(0).toString();
        QString usage_time = query.value(1).toString();
        res.insert(usage_date, usage_time);
    }

}
