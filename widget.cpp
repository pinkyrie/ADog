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
#include <QFileInfo>
#include <QDir>
#include <system_error>
#include <shobjidl.h>
#include <propkey.h>
#include <comdef.h>
#include <atlbase.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    LoadAppDict();
    connect(SnapTimer, &QTimer::timeout, this, [=](){
        RecordTime(StartTime);
                                               });
    SnapTimer->start();

}
void Widget::LoadAppDict()
{
    QFile file("./usage_data.json");
    qDebug() << QCoreApplication::applicationDirPath();
    QDir appDir(QCoreApplication::applicationDirPath());

    // 导航到项目根目录
    appDir.cdUp(); // 进入 build 目录
    appDir.cdUp(); // 进入 ADog 目录

    // 构建相对路径
    QString relativeFilePath = appDir.absoluteFilePath("D:/soft/Qt/projects/ADog/test.json");
    qDebug() << relativeFilePath;
    qDebug() << file.exists();
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "read error";
        return;
    }
    // QByteArray fileData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QMap<QString, int> map;

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        qDebug() << "Key:" << it.key() << "Value:" << it.value();

        // 输出值的类型
        if (it.value().isDouble()) {
            qDebug() << "Type: Double";
            map.insert(it.key(), it.value().toInt());
        } else if (it.value().isString()) {
            qDebug() << "Type: String";
            bool ok;
            int intValue = it.value().toString().toInt(&ok);
            if (ok) {
                map.insert(it.key(), intValue);
            } else {
                qWarning() << "Invalid integer value in string for key" << it.key();
            }
        } else if (it.value().isBool()) {
            qDebug() << "Type: Bool";
            map.insert(it.key(), it.value().toBool() ? 1 : 0);
        } else if (it.value().isNull()) {
            qDebug() << "Type: Null";
            qWarning() << "Null value for key" << it.key();
        } else {
            qWarning() << "Invalid value type for key" << it.key();
        }
    }


}

QString Widget:: GetCurrentApp(){
    HWND hwnd = GetForegroundWindow();
    return GetWindowTitle(hwnd);


}

void Widget::SavaUsageApps()
{

}

void Widget::RecordTime(QDateTime StartTime)
{
    QDateTime EndTime = StartTime;

    HWND hwnd = GetForegroundWindow();
    QString CurrentWindow = getProcessDescription(hwnd);
    qDebug() << "window name is:" << CurrentWindow;
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

// quote from Mrbean C huge thanks to mrbeanC :)
QString Widget::getFileDescription(const QString& path)
{
    CoInitialize(nullptr); // 初始化 COM 库

    std::wstring wStr = path.toStdWString();
    LPCWSTR pPath = wStr.c_str();

    // 使用 CComPtr 自动释放 IShellItem2 接口
    CComPtr<IShellItem2> pItem;
    HRESULT hr = SHCreateItemFromParsingName(pPath, nullptr, IID_PPV_ARGS(&pItem));
    if (FAILED(hr))
        throw std::system_error(hr, std::system_category(), "SHCreateItemFromParsingName() failed");


    QString desc { QFileInfo(path).fileName() }; // default | fallback

    // 使用 CComHeapPtr 自动释放字符串（调用 CoTaskMemFree）
    CComHeapPtr<WCHAR> pValue;
    hr = pItem->GetString(PKEY_FileDescription, &pValue);
    if (SUCCEEDED(hr))
        desc = QString::fromWCharArray(pValue);
    else
        qDebug() << "No description there. Fallback to exe filename:" << desc;

    CoUninitialize(); // 取消初始化 COM 库
    return desc;
}

// quote from Mrbean C huge thanks to mrbeanC :)
QString Widget::getProcessExePath(HWND hwnd)
{
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId == 0)
        return "";

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL)
        return "";

    TCHAR processName[MAX_PATH] = TEXT("<unknown>");
    // https://www.cnblogs.com/mooooonlight/p/14491399.html
    if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processName);
    }

    CloseHandle(hProcess);
    return "";
}

// quote from Mrbean C huge thanks to mrbeanC :)
QString Widget::getProcessDescription(HWND hwnd)
{
    QString exePath = getProcessExePath(hwnd);
    return getFileDescription(exePath);
}

Widget::~Widget()
{
    delete ui;
}


