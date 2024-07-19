#ifndef WIDGET_H
#define WIDGET_H

#include "dbmanager.h"
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QtCharts/QChartView>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    QtCharts::QChartView *chartView;
    ~Widget();

private:
    Ui::Widget *ui;
    QTimer * SnapTimer;
    QDateTime StartTime;
    DBManager DBmanager;
    std::vector<QString> AppList = {"qtcreator.exe"};
    std::map<QString, int> AppUsageDict= {{"qtcreator.exe", 3600},
                                   {"ADog.exe", 2400}};
    QMap<QString, QString> resByAppName;
    QMap<QString, QString> resByDate;
    QString RecordingWindow = nullptr;
    int Interval = 1000;
    void LoadAppDict();
    void InitAppDict();
    bool AddApp(const QString& appName);
    bool UpdateAppUsage(const QString& appName);
    bool DeleteApp(const QString& appName);
    bool GetAppUsageTime(const QString& appName);
    QString GetCurrentApp();
    QString getFileDescription(const QString& path);
    QString getProcessExePath(HWND hwnd);
    QString getProcessDescription(HWND hwnd);
    void SavaUsageApps();
    void RecordTime(QDateTime StartTime);
    QString GetWindowTitle(HWND hwnd);
};
#endif // WIDGET_H
