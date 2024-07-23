#include "widget.h"
#include "iconlabel.h"
#include "qbarcategoryaxis.h"
#include "qboxlayout.h"
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
#include <QtWinExtras>
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
    // connect(ui->btn_test, &QPushButton::clicked, this, [=]() {
    //     qDebug() << "clicked" << QTime::currentTime() << "\n";
    // });
    SnapTimer->setInterval(Interval);

    LoadAppDict();
    SaveAppIcon();

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

    QHBoxLayout *topLayout = new QHBoxLayout;
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);


    QPushButton *button1 = new QPushButton("Button 1", this);
    QPushButton *button2 = new QPushButton("Button 2", this);
    QPushButton *button3 = new QPushButton("Button 3", this);
    topLayout->addWidget(button1);
    topLayout->addWidget(button2);
    topLayout->addWidget(button3);

    QString appName1 = "Qt Creator";
    QString dir = QCoreApplication::applicationDirPath();
    QString path1 = dir + "/png/" + appName1 + ".png";
    QString path2 = path1;
    QString path3 = path1;

    QGridLayout *bottomLayout = new QGridLayout(this);
    bottomLayout->setSpacing(10);  // 设置图标之间的间距

    QStringList iconSets = {path1, path2, path3, path3, path3, path3};
    for (int i = 0; i < iconSets.count(); i++){
        QPixmap iconPixmap(iconSets[i]);
        IconLabel * iconLabel = new IconLabel(iconPixmap, i, this);
        connect(iconLabel, &IconLabel::clicked, this, &Widget::onIconClicked);
        iconLabels.append(iconLabel);

        auto bar = new QBarSet("xxx");
        *bar << i;
        auto chart = new QChart;
        auto HBarseries = new QHorizontalBarSeries;
        HBarseries->append(bar);
        chart->addSeries(HBarseries);

        auto chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setFixedHeight(100);

        QStringList category;
        category << "";
        QBarCategoryAxis *axisL = new QBarCategoryAxis();
        axisL->append(category);
        chart->addAxis(axisL, Qt::AlignLeft);
        HBarseries->attachAxis(axisL);

        QValueAxis *axisB = new QValueAxis();
        axisB->setRange(0,15);
        chart->addAxis(axisB, Qt::AlignBottom);
        HBarseries->attachAxis(axisB);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignLeft);

        bottomLayout->addWidget(iconLabel, i ,0);
        bottomLayout->addWidget(chartView, i ,1);


    }
    QWidget *scrollWidget = new QWidget;
    scrollWidget->setLayout(bottomLayout);
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedHeight(400);  // 设置滚动区域的固定高度

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addItem(verticalSpacer);
    mainLayout->addWidget(scrollArea);

    setLayout(mainLayout);

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

QPixmap Widget::GetApplicationIcon(const QString &exePath)
{
    std::wstring exePathW = exePath.toStdWString();
    LPCWSTR exePathLPCWSTR = exePathW.c_str();

    // Get the icon from the executable
    HICON hIcon = ExtractIconW(NULL, exePathLPCWSTR, 0);

    if (hIcon == NULL || hIcon == (HICON)1) {
        qDebug() << "无法提取图标";
        return QPixmap();
    }

    // Convert HICON to QPixmap
    QPixmap pixmap = QtWin::fromHICON(hIcon);

    // Destroy the icon to free resources
    DestroyIcon(hIcon);

    return pixmap;
}

bool Widget::SaveAppIcon()
{
    HWND hwnd = GetForegroundWindow();//TODO:hwnd做参数
    QString exePath = getProcessExePath(hwnd);
    QPixmap iconPixmap = GetApplicationIcon(exePath);
    QString appName = getProcessDescription(hwnd);
    QString appDir = QCoreApplication::applicationDirPath();
    QDir dir(appDir);
    if (!dir.exists("png")) {
        if (!dir.mkdir("png")) {
            qDebug() << "无法创建 png 文件夹";
            return -1;
        }
    }
    if (!iconPixmap.isNull()) {
        // 保存 QPixmap 为 PNG 文件
        QString filePath = appDir + "/png/" + appName + ".png";
        if (iconPixmap.save(filePath, "PNG")) {
            qDebug() << "图标已保存为:" << filePath;
            return true;
        } else {
            qDebug() << "无法保存图标为 PNG 文件";
        }
    } else {
        qDebug() << "无法提取图标";
    }
    return false;


}

Widget::~Widget()
{
    delete ui;
}

void Widget::onIconClicked(int index)
{
    qDebug() << "label: " << index << "is clicked";
}


