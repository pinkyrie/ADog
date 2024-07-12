#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QDebug>
#include <QTime>
#include <Windows.h>
#include <WinUser.h>
#include <Psapi.h>
#include <libloaderapi.h>
#include <QFile>

Widget::Widget(QWidget *parent)
    : QWidget(parent),ui(new Ui::Widget),
    SnapTimer(new QTimer(this)),
    StartTime(QDateTime::currentDateTime())
{
    ui->setupUi(this);

    setWindowTitle("ADog - Your App Usage Watch Dog");
    connect(ui->btn_test, &QPushButton::clicked, this, [=]() {
        qDebug() << "clicked" << QTime::currentTime() << "\n";
    });
    SnapTimer->setInterval(Interval);
    connect(SnapTimer, &QTimer::timeout, this, [=](){
        RecordTime(StartTime);
                                               });
    SnapTimer->start();

}
void Widget::LoadAppDict()
{
    // QFile loadFile(saveFormat == JsonQFile loadFile(saveFormat == Json)
}

void Widget:: GetTabApps(){
    HWND hwnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    while (hwnd) {
        if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0) {

        }
        hwnd = GetWindow(hwnd, GW_HWNDNEXT);
    }


}

void Widget::SavaUsageApps()
{

}

void Widget::RecordTime(QDateTime StartTime)
{
    QDateTime EndTime = StartTime;
    HWND hwnd = GetForegroundWindow();
    QString CurrentWindow = GetWindowTitle(hwnd);
    auto iter = AppUsageDict.find(CurrentWindow);
    if (iter != AppUsageDict.end()) {
        if (RecordingWindow == nullptr) {
            RecordingWindow = CurrentWindow;
        }
        else if (RecordingWindow == CurrentWindow) {
            iter -> second += Interval/1000;
        }
        RecordingWindow = CurrentWindow;
    }
    else{
        qDebug() << "Not found in the App recording list";
        return;
    }
    qDebug() << iter->first << "time: " << iter->second;
}
QString Widget:: GetWindowTitle(HWND hwnd) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                      PROCESS_VM_READ,
                                  FALSE, pid);
    QString processNameStr;
    if (hProcess != nullptr) {
        TCHAR processName[MAX_PATH] = TEXT("<unknown>");
        if (GetModuleBaseName(hProcess, nullptr, processName, MAX_PATH)) {
            processNameStr = QString::fromWCharArray(processName);
            qDebug() << "Process Name: " << processNameStr << "\n";
        }
        CloseHandle(hProcess);
    }
    return processNameStr;
}



Widget::~Widget()
{
    delete ui;
}


