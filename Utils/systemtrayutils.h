#ifndef SYSTEMTRAYUTILS_H
#define SYSTEMTRAYUTILS_H

#include <QObject>
#include <QSystemTrayIcon>

class SystemTrayUtils : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayUtils(QObject *parent = nullptr);

signals:
};

#endif // SYSTEMTRAYUTILS_H
