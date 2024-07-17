#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class DBManager
{
public:
    DBManager();
    bool createItem(const QString& appName);
    bool updateItem(const QString& appName,
                    const QString& date,
                    const QString& usageTime);
    void readByDate(const QString& date,
                    QMap<QString, QString>& res);
    void readByAppName(const QString& appName,
                    QMap<QString, QString>& res);

};

#endif // DBMANAGER_H
