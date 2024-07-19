#include "widget.h"
#include <QVBoxLayout>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QVBoxLayout *layout = new QVBoxLayout;
    Widget w;
    layout->addWidget(w.chartView);

    // 设置窗口的主布局
    w.setLayout(layout);
    w.resize(800, 600);
    //w.resize(420, 300);
    w.show();
    return a.exec();
}
