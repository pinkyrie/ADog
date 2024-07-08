#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QDebug>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("ADog - Your App Usage Watch Dog");
    connect(ui->btn_test, &QPushButton::clicked, this, [=]() {
        qDebug() << "clicked" << QTime::currentTime();
    });
}

Widget::~Widget()
{
    delete ui;
}
