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
    int interval = 1000;
    SnapTimer->setInterval(interval);
    connect(SnapTimer, &QTimer::timeout, this, [=](){
        qDebug() << QTime::currentTime();
                                               });
    SnapTimer->start();

}

void Widget:: RecordTabApps(){
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

void Widget::RecordTime()
{
    HWND hwnd = GetForegroundWindow();
    QString currentWindowName = GetWindowTitle(hwnd);
    qDebug() << "current title is:" << currentWindowName;
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
