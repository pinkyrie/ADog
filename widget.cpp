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

#include "Utils/AppUtil.h"
#include "Utils/Util.h"
QT_CHARTS_USE_NAMESPACE

Widget::Widget(QWidget *parent)
    : QWidget(parent),ui(new Ui::Widget),
    SnapTimer(new QTimer(this)),
    StartTime(QDateTime::currentDateTime()),
    DBmanager()
{
    InitDate = QDate::currentDate().toString("yyyy-MM-dd");
    ui->setupUi(this);
    ui->lineEdit->setText(InitDate);
    setWindowTitle("ADog - Your App Usage Watch Dog");
    bottomLayout->setSpacing(10);
    ShowChart();
    SnapTimer->setInterval(Interval);

    connect(SnapTimer, &QTimer::timeout, this, [=](){
        RecordTime(StartTime);
        SaveAppIcon();
    });
    connect(ui->LeftBtn, &QPushButton::clicked, this, [=](){
        ShowDate = ShowDate.addDays(-1);
        ui->lineEdit->setText(ShowDate.toString("yyyy-MM-dd"));
        ShowChart();
        qDebug() << "date" << ShowDate;
    });
    connect(ui->RightBtn, &QPushButton::clicked, this, [=](){
        if (ShowDate < QDate::currentDate()) {
            ShowDate = ShowDate.addDays(+1);
            ui->lineEdit->setText(ShowDate.toString("yyyy-MM-dd"));
            ShowChart();
            qDebug() << "date" << ShowDate;
        }
    }
);
    SnapTimer->start();
    sysTray = new SystemTrayUtils(this);
    sysTray->show();
}

//deprecated: 不采用json的方式读取数据，sqlite更适合存储和条件查询带有日期等信息的数据
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
}

void Widget::InitAppDict()
{
    // 获取今日日期
    // 格式化日期
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    qDebug() << "today is:" << today;
    QString date = "2024-07-16";
    qDebug() << "date";
    DBmanager.readByDate(today, resByDate);
    for (auto it = resByDate.begin(); it != resByDate.end(); ++it) {
        qDebug() << "Key:" << it.key() << "Value:" << it.value();
    }

}


void Widget::UpdateChart()
{
    DBmanager.readByDate(InitDate, resByDate);

}

bool Widget::DeleteApp(const QString &appName)
{
    return true;
}



void Widget::RecordTime(QDateTime StartTime)
{
    QDateTime EndTime = StartTime; //TODO：last use time

    HWND hwnd = GetForegroundWindow();
    QString CurrentWindow = getProcessDescription(hwnd);
    qDebug() << "window name is:" << CurrentWindow;
    auto iter = AppUsageDict.find(CurrentWindow);
    if (iter != AppUsageDict.end()) {
        if (RecordingWindow == nullptr) {
            RecordingWindow = CurrentWindow;
        }
        else if (RecordingWindow == CurrentWindow) {
            iter.value() += Interval/1000;
        }
        RecordingWindow = CurrentWindow;
    }
    else{
        AppUsageDict.insert(CurrentWindow, 0);
        qDebug() << "Not found in the App recording list";
        return;
    }
    qDebug() << iter.key() << "time: " << iter.value();
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



void Widget::UpdateUsage()
{
    for (auto it = AppUsageDict.begin(); it != AppUsageDict.end(); ++it) {
        QString appName = it.key();
        int usageTime = it.value();
        int resId = DBmanager.resIdPerAppDate(appName, InitDate);
        qDebug() << appName <<":" << resId;
        if(resId != -1){
            DBmanager.updateItem(resId, InitDate, usageTime);
        }
        else{
            DBmanager.createItem(appName, InitDate, usageTime);
        }
    }

}

bool Widget::CheckSaving()
{
    QString RecordingDate = QDate::currentDate().toString("yyyy-MM-dd");
    UpdateUsage();
    if (RecordingDate != InitDate){
        InitDate = RecordingDate;
    }
    return true;
}

void Widget::ShowChart()
{
    QString dir = QCoreApplication::applicationDirPath();
    QString ShowDateStr = ShowDate.toString("yyyy-MM-dd");
    if(!resByDate.empty()){
        resByDate.clear();
    }
    DBmanager.readByDate(ShowDateStr, resByDate);
    // 换页进行一波清空
    QLayoutItem *child;
    while ((child = bottomLayout->takeAt(0)) != nullptr) {
        // scrollWidget->hide();
        bottomLayout->removeItem(child);
        if (child->widget()) {
            delete child->widget(); // Ensure proper deletion of widgets
        }
        delete child; // Delete the layout item
    }
    // scrollWidget->show();
    qDebug() << "layout count after delete is: " << bottomLayout->count();
    int index = 0;
    if(resByDate.count() == 0){
        return ;
    }
    QTimeLine* timeLine = new QTimeLine(150, this);
    for(auto it = resByDate.begin(); it != resByDate.end(); it++, index++){
        auto appName = it.key();
        auto usageTime = it.value();
        auto bar = new QBarSet(appName);
        //读取app的图标
        QString path = dir + "/png/" + appName + ".png";
        // 检查文件是否存在
        if (!QFile::exists(path)) {
            // 如果文件不存在，使用默认图片路径
            path = ":/images/default.png";
        }
        QPixmap iconPixmap(path);
        IconLabel * iconLabel = new IconLabel(iconPixmap, appName, scrollArea); //数字是为了开发过程标注哪一个被点击

        timeLine->setUpdateInterval(10); //default 40ms 25fps
        connect(timeLine, &QTimeLine::frameChanged, this, &Widget::setFixedWidth);
        connect(iconLabel, &IconLabel::clicked, [=](bool checked) { //hhh
            if(!resByAppName.empty()){
                resByAppName.clear();
            }
            if(!resByAppNameFiltered.empty()){
                resByAppNameFiltered.clear();
            }
            QLayoutItem *child;
            while ((child = ui->RightLayout->takeAt(1)) != nullptr) {
                ui->RightLayout->removeItem(child);
                if (child->widget()) {
                    delete child->widget(); // Ensure proper deletion of widgets
                }
                delete child; // Delete the layout item
            }
            DBmanager.readByAppName(iconLabel->appName, resByAppName);
            qDebug() << "name" << iconLabel->appName;
            for(auto it = resByAppName.begin(); it != resByAppName.end(); it++){
                qDebug() << "res app name" << it.key() << it.value();
            }
            timeLine->stop(); //stop whenever click
            if (checked) {
                //setFixedWidth(Normal_W + Extra_W);
                timeLine->setFrameRange(width(), 540 + 400);


                // Create the bar series
                QBarSeries *series = new QBarSeries();

                // Create a bar set and add data to it
                QBarSet *set = new QBarSet("Usage Time (minutes)");

                QStringList categories;
                QDate startOfWeek, endOfWeek;
                getCurrentWeekStartEnd(startOfWeek, endOfWeek);
                qDebug() << "before filtering";
                filterUsageTimeForCurrentWeek(resByAppName, resByAppNameFiltered);
                for(auto it = resByAppNameFiltered.begin(); it != resByAppNameFiltered.end(); it++){
                    qDebug() << "filter app name" << it.key() << it.value();
                }
                qDebug() << "start week end week" << startOfWeek << endOfWeek;
                for (QDate date = startOfWeek; date <= endOfWeek; date = date.addDays(1)) {
                    categories << dayNames[date.dayOfWeek() - 1];
                    if (resByAppNameFiltered.contains(date)) {
                        *set << resByAppNameFiltered.value(date).toInt()/60;
                    }else{
                        *set << 0;
                    }
                }
                series->append(set);

                // Create the chart
                QChart *chart = new QChart();
                chart->addSeries(series);
                chart->setTitle("Weekly Usage Time");
                chart->setAnimationOptions(QChart::SeriesAnimations);

                // Create the axis
                QBarCategoryAxis *axisX = new QBarCategoryAxis();
                axisX->append(categories);
                chart->addAxis(axisX, Qt::AlignBottom);
                series->attachAxis(axisX);

                QValueAxis *axisY = new QValueAxis();
                axisY->setTitleText("Usage Time (minutes)");
                chart->addAxis(axisY, Qt::AlignLeft);
                series->attachAxis(axisY);

                chart->legend()->setVisible(true);
                chart->legend()->setAlignment(Qt::AlignBottom);

                // Create the chart view
                QChartView *chartView = new QChartView(chart);
                chartView->setRenderHint(QPainter::Antialiasing);
                ui->RightLayout->addWidget(chartView);
            } else {
                //setFixedWidth(Normal_W);
                timeLine->setFrameRange(width(), 540);
                //QTimer::singleShot(100, [=]() { writeSetting(); });//I/O会阻塞动画，移至finished↑
            }

            timeLine->start();
        });
        // TODO: 手动绘制七天内的app使用情况横向为日期 纵向显示使用时间chart
        iconLabels.append(iconLabel);
        //绘制柱状图
        *bar << usageTime.toInt()/60;
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

        bottomLayout->addWidget(iconLabel, index ,0);
        bottomLayout->addWidget(chartView, index ,1);
    }
    QWidget *scrollWidget = new QWidget(this);

    scrollWidget->setLayout(bottomLayout);

    // scrollArea->setWidget(scrollWidget);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    // scrollArea->setFixedHeight(450);  // 设置滚动区域的固定高度
    // scrollArea->setFixedWidth(600);
    ui->BottomLayout->addWidget(scrollArea);

    mainLayout->addLayout(ui->TopLayout);

    // mainLayout->addWidget(scrollArea);
    mainLayout->addLayout(ui->BottomLayout);
    mainLayout->addLayout(ui->RightLayout);

    setLayout(mainLayout);

}

void Widget::closeEvent(QCloseEvent *event)
{
    if (CheckSaving()) {
        // If the save operation is successful, accept the close event
        event->accept();
    } else {
        // If the save operation fails, ignore the close event and inform the user
        qDebug() << "close fails to save data";
        event->ignore();
    }
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
    if (AppUtil::isAppFrameWindow(hwnd)) // UWP特殊处理
        hwnd = AppUtil::getAppCoreWindow(hwnd); // AppCore 用于获取exe路径
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
    HWND hwnd = GetForegroundWindow();// TODO:hwnd做参数
    QString exePath = getProcessExePath(hwnd);
    // QPixmap iconPixmap = GetApplicationIcon(exePath);
    QPixmap iconPixmap = Util::getCachedIcon(exePath,hwnd).pixmap(64,64);
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

void Widget::getCurrentWeekStartEnd(QDate &startOfWeek, QDate &endOfWeek) {
    QDate currentDate = QDate::currentDate();
    startOfWeek = currentDate.addDays(-currentDate.dayOfWeek() + 1); // Assuming week starts on Monday
    endOfWeek = startOfWeek.addDays(6); // Week ends on Sunday
}

void Widget::filterUsageTimeForCurrentWeek(const QMap<QDate, QString> &usageTimeMap, QMap<QDate, QString> &filteredMap) {
    QDate startOfWeek, endOfWeek;
    qDebug() << "filter";
    getCurrentWeekStartEnd(startOfWeek, endOfWeek);
    qDebug() << "count" << usageTimeMap.count();
    for (auto it = usageTimeMap.begin(); it != usageTimeMap.end(); ++it) {
        qDebug() << "filtering" << it.key() << it.value();
        if (it.key() >= startOfWeek && it.key() <= endOfWeek) {
            filteredMap.insert(it.key(), it.value());
            qDebug() << "filter date" << it.key() << it.value();
        }
    }

}
Widget::~Widget()
{
    delete ui;
}

void Widget::onIconClicked(int index)
{
    qDebug() << "label: " << index << "is clicked";
}


