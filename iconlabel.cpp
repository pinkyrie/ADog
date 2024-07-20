#include "iconlabel.h"

IconLabel::IconLabel(const QPixmap &pixmap,
                     int index, QWidget *parent): QLabel(parent), index(index)
{
    setPixmap(pixmap);
    setFixedSize(pixmap.size());
    setCursor(Qt::PointingHandCursor);
}
