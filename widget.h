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
    void RecordTabApps();
    void SavaUsageApps();
    void RecordTime();
    QString GetWindowTitle(HWND hwnd);
};
#endif // WIDGET_H
