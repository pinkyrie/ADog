#include "iconlabel.h"

IconLabel::IconLabel(const QPixmap &pixmap,
                    QString appName,
                    QWidget *parent):
    QLabel(parent), appName(appName), checked(true)
{
    setPixmap(pixmap);
    setFixedSize(pixmap.size());
    setCursor(Qt::PointingHandCursor);
}
