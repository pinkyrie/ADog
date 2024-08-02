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

    QTimer * SnapTimer;
    QDateTime StartTime;
    DBManager DBmanager;
    QVector<IconLabel *> iconLabels;
    QWidget *scrollWidget = new QWidget;
    QGridLayout *bottomLayout = new QGridLayout(scrollWidget);
    QScrollArea *scrollArea = new QScrollArea;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    std::vector<QString> AppList = {"qtcreator.exe"};
    QMap<QString, int> AppUsageDict;
    QMap<QString, QString> resByAppName;
    QMap<QString, QString> resByDate;
    QString RecordingWindow = nullptr;
    QString InitDate = nullptr;
    QDate ShowDate = QDate::currentDate();
    int Interval = 1000;
    void LoadAppDict();
    void InitAppDict();
    bool AddApp(const QString& appName);
    void UpdateChart();
    bool DeleteApp(const QString& appName);
    bool GetAppUsageTime(const QString& appName);
    QString GetCurrentApp();
    QString getFileDescription(const QString& path);
    QString getProcessExePath(HWND hwnd);
    QString getProcessDescription(HWND hwnd);
    QPixmap GetApplicationIcon(const QString &exePath);
    bool SaveAppIcon();
    void SavaUsageApps();
    void RecordTime(QDateTime StartTime);
    QString GetWindowTitle(HWND hwnd);

    void SaveUsage();
    void UpdateUsage();
    bool CheckSaving();
    void ShowChart();
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // WIDGET_H
