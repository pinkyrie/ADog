#ifndef ICONLABEL_H
#define ICONLABEL_H

#include "qevent.h"
#include <QLabel>

class IconLabel : public QLabel
{
   Q_OBJECT
public:
   IconLabel(const QPixmap &pixmap,
             QString appName,
             QWidget *parent = nullptr);
    QString appName;
signals:
    void clicked(bool checked);
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if ( event ->button() == Qt::LeftButton) {
            emit clicked(checked);
            checked = !checked;
        }
        QLabel::mousePressEvent(event);
    }

private:
    bool checked;

};

#endif // ICONLABEL_H
