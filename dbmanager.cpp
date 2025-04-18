#include "dbmanager.h"
#include "qcoreapplication.h"
#include "qdatetime.h"

DBManager::DBManager() {
    // 获取当前应用程序所在目录
    QString appDir = QCoreApplication::applicationDirPath();
    QString dbPath = appDir + "/database/" + "/usageRecord.db";

    // 设置数据库路径
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbPath);
    if (!database.open())
    {
        qDebug() << "db path: " << dbPath;
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

bool DBManager::createItem(const QString &appName, const QString &date, int usageTime)
{

    QSqlQuery query;
    query.prepare("INSERT INTO usage (app_name, usage_date, usage_time) "
                  "VALUES (:app_name, :usage_date, :usage_time)");
    query.bindValue(":app_name", appName);
    query.bindValue(":usage_date", date);
    query.bindValue(":usage_time", usageTime);
    if(query.exec()){
        return true;
    }
    else{
        qDebug() << "create fails";
        return false;
    }
}

bool DBManager::updateItem(int id, const QString &date, int usageTime)
{
    QSqlQuery query;

    // Step 1: Retrieve the existing usage_time
    query.prepare("SELECT usage_time FROM usage WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "Query execution error: " << query.lastError().text();
        qDebug() << "error time0";
        return false;
    }

    if (!query.next()) {
        qDebug() << "No record found with id:" << id;
        return false;
    }

    int currentUsageTime = query.value(0).toInt();
    qDebug() << "error time";
    // Step 2: Update the usage_time by adding the passed usageTime
    int updatedUsageTime = currentUsageTime + usageTime;

    query.prepare("UPDATE usage SET usage_date = :usage_date, usage_time = :usage_time "
                  "WHERE id = :id");
    query.bindValue(":usage_time", updatedUsageTime);
    query.bindValue(":usage_date", date);
    query.bindValue(":id", id);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Update fails: " << query.lastError().text();
        return false;
    }
}

void DBManager::readByDate(const QString &date, QMap<QString, QString> &res)
{
    QSqlQuery query;
    query.prepare("SELECT app_name, usage_time "
                  "FROM usage WHERE usage_date = :date");
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

void DBManager::readByAppName(const QString &appName, QMap<QDate, QString> &res)
{
    QSqlQuery query;
    query.prepare("SELECT usage_date, usage_time "
                  "FROM usage WHERE app_name = :appName");
    query.bindValue(":appName", appName);
    if (!query.exec()) {
        qDebug() << "Query execution error: " << query.lastError().text();
        return;
    }

    // 读取查询结果并存储到res中
    while (query.next()) {
        QDate usage_date = query.value(0).toDate();;
        QString usage_time = query.value(1).toString();
        res.insert(usage_date, usage_time);
    }

}

int DBManager::resIdPerAppDate(const QString &appName, const QString &date)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM usage WHERE app_name = :appName AND usage_date = :date");
    query.bindValue(":appName", appName);
    query.bindValue(":date", date);

    if (!query.exec()) {
        qDebug() << "Query execution error: " << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value("id").toInt();
    } else {
        return -1; // No matching record found
    }

}
