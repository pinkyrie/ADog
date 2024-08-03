#include "iconlabel.h"

IconLabel::IconLabel(const QPixmap &pixmap,
                     bool checked, QWidget *parent): QLabel(parent), checked(true)
{
    setPixmap(pixmap);
    setFixedSize(pixmap.size());
    setCursor(Qt::PointingHandCursor);

}
