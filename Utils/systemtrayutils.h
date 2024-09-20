#ifndef SYSTEMTRAYUTILS_H
#define SYSTEMTRAYUTILS_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QAction>
#include <QApplication>
#include <QMenu>

class SystemTrayUtils : public QSystemTrayIcon
{
    Q_OBJECT

public:
    SystemTrayUtils(QWidget *parent = nullptr);

private:
    void setMenu(QWidget* parent = nullptr);

signals:



};

#endif // SYSTEMTRAYUTILS_H
