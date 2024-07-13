#ifndef WIDGET_H
#define WIDGET_H

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

private:
    Ui::Widget *ui;
    QTimer * SnapTimer;
    QDateTime StartTime;
    std::vector<QString> AppList = {"qtcreator.exe"};
    std::map<QString, int> AppUsageDict= {{"qtcreator.exe", 3600},
                                        {"ADog.exe", 2400}};
    QString RecordingWindow = nullptr;
    int Interval = 1000;
    void LoadAppDict();
    QString GetCurrentApp();
    QString getFileDescription(const QString& path);
    QString getProcessExePath(HWND hwnd);
    QString getProcessDescription(HWND hwnd);
    void SavaUsageApps();
    void RecordTime(QDateTime StartTime);
    QString GetWindowTitle(HWND hwnd);
};
#endif // WIDGET_H
