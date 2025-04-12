#include "iconlabel.h"

IconLabel::IconLabel(const QPixmap &pixmap,
                    QString appName,
                    QWidget *parent):
    QLabel(parent), appName(appName), checked(true)
{
    auto new_pixmap = pixmap.scaled(48,48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(new_pixmap);
    setFixedSize(new_pixmap.size());
    setCursor(Qt::PointingHandCursor);
}
