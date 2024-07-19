#include "widget.h"
#include "qbarcategoryaxis.h"
#include "qvalueaxis.h"
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
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QHorizontalBarSeries>

QT_CHARTS_USE_NAMESPACE

Widget::Widget(QWidget *parent)
    : QWidget(parent),ui(new Ui::Widget),
    SnapTimer(new QTimer(this)),
    StartTime(QDateTime::currentDateTime()),
    DBmanager()
{
    ui->setupUi(this);
    setWindowTitle("ADog - Your App Usage Watch Dog");

    InitAppDict();
    AddApp("app");
    GetAppUsageTime("qtcreator");
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
    // 需要把ini文件放到和可执行文件exe同级下
    QString filePath = QCoreApplication::applicationDirPath() + "/usage_data.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "read error";
        qDebug() << file.exists();
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

    // for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
    //     qDebug() << "Key:" << it.key() << "Value:" << it.value();
    //     qDebug() << it.value().toString();
    //     qDebug() << it.value().toString().toInt();
    // }


}

void Widget::InitAppDict()
{
    // 获取今日日期
    // 格式化日期
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    qDebug() << "today is:" << today;
    QString date = "2024-07-16";
    DBmanager.readByAppName("qtcreator", resByAppName);
    for (auto it = resByAppName.begin(); it != resByAppName.end(); ++it) {
        qDebug() << "Key:" << it.key() << "Value:" << it.value();
    }
    qDebug() << "date";
    DBmanager.readByDate(date, resByDate);
    for (auto it = resByDate.begin(); it != resByDate.end(); ++it) {
        qDebug() << "Key:" << it.key() << "Value:" << it.value();
    }

}

bool Widget::AddApp(const QString &appName)
{
    auto set0 = new QBarSet("Jane");
    auto set1 = new QBarSet("John");
    auto set2 = new QBarSet("Axel");
    auto set3 = new QBarSet("Mary");
    auto set4 = new QBarSet("Sam");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
    *set4 << 9 << 7 << 5 << 3 << 1 << 2;

    auto barseries = new QHorizontalBarSeries;
    barseries->append(set0);
    barseries->append(set1);
    barseries->append(set2);
    barseries->append(set3);
    barseries->append(set4);

    auto chart = new QChart;
    chart->addSeries(barseries);

    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    auto axisX = new QBarCategoryAxis;
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignLeft);
    barseries->attachAxis(axisX);
    axisX->setRange(QString("Jan"), QString("Jun"));

    auto axisY = new QValueAxis;
    chart->addAxis(axisY, Qt::AlignBottom);
    barseries->attachAxis(axisY);
    axisY->setRange(0, 20);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignLeft);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return true;
}

bool Widget::UpdateAppUsage(const QString &appName)
{
    return true;
}

bool Widget::DeleteApp(const QString &appName)
{
    return true;
}

bool Widget::GetAppUsageTime(const QString &appName)
{
    QSqlQuery query("SELECT * FROM AppUsage");
    while (query.next()) {
        QString appName = query.value(1).toString();
        qDebug() << appName;
    }
    return appName != nullptr;
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


