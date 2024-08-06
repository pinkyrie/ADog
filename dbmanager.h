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
    bool createItem(const QString& appName, const QString& date, int usageTime);
    bool updateItem(int id, const QString& date, int usageTime);
    void readByDate(const QString& date,
                    QMap<QString, QString>& res);
    void readByAppName(const QString& appName,
                    QMap<QDate, QString>& res);
    int resIdPerAppDate(const QString& appName, const QString& date);

};

#endif // DBMANAGER_H
