#ifndef ICONLABEL_H
#define ICONLABEL_H

#include "qevent.h"
#include <QLabel>

class IconLabel : public QLabel
{
   Q_OBJECT
public:
    IconLabel(const QPixmap &pixmap,
              int index,
              QWidget *parent = nullptr);
signals:
    void clicked(int index);
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if ( event ->button() == Qt::LeftButton) {
            emit clicked(index);
        }
        QLabel::mousePressEvent(event);
    }

private:
    int index;
};

#endif // ICONLABEL_H
