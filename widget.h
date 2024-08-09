#ifndef WIDGET_H
#define WIDGET_H

#include "dbmanager.h"
#include "iconlabel.h"
#include "qgridlayout.h"
#include "qscrollarea.h"
#include <QWidget>
#include <QTimer>
#include <QDateTime>
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
    ~Widget();

private slots:
    void onIconClicked(int index);

private:
    Ui::Widget *ui;
    // ui widgets 提前放到类中作为成员 避免每次更新视图都会重复生成所有widget
    const QStringList dayNames = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    int Interval = 1000;
    QTimer * SnapTimer;
    QDateTime StartTime;
    DBManager DBmanager;

    QVector<IconLabel *> iconLabels;
    QWidget *scrollWidget = new QWidget;
    QGridLayout *bottomLayout = new QGridLayout(scrollWidget);
    QScrollArea *scrollArea = new QScrollArea;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QMap<QString, int> AppUsageDict;
    QMap<QDate, QString> resByAppName;
    QMap<QString, QString> resByDate;
    QMap<QDate, QString> resByAppNameFiltered;

    QString RecordingWindow = nullptr;
    QString InitDate = nullptr;
    QDate ShowDate = QDate::currentDate();


    void LoadAppDict();
    void InitAppDict();
    void UpdateChart();
    bool DeleteApp(const QString& appName);


    QString GetCurrentApp();
    QString getFileDescription(const QString& path);
    QString getProcessExePath(HWND hwnd);
    QString getProcessDescription(HWND hwnd);
    QPixmap GetApplicationIcon(const QString &exePath);
    QString GetWindowTitle(HWND hwnd);

    bool SaveAppIcon();
    void RecordTime(QDateTime StartTime);
    void UpdateUsage();
    bool CheckSaving();
    void ShowChart();
    void getCurrentWeekStartEnd(QDate &startOfWeek, QDate &endOfWeek);
    void filterUsageTimeForCurrentWeek(const QMap<QDate, QString> &usageTimeMap, QMap<QDate, QString> &filteredMap);

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // WIDGET_H
