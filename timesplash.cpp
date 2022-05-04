#include <QEvent>
#include <QDebug>

#include "timesplash.h"

TimeSplash::TimeSplash(const QPixmap &pixmap, Qt::WindowFlags f)
{
    this->setPixmap(pixmap);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    this->installEventFilter(this);
}

bool TimeSplash::eventFilter(QObject *target, QEvent *event)
{
    Q_UNUSED(target)
    if((event->type() == QEvent::MouseButtonPress) ||
       (event->type() == QEvent::MouseButtonRelease) ||
       (event->type() == QEvent::KeyPress) ||
       (event->type() == QEvent::KeyRelease)) {
        return true;
    }

    if(event->type() == QEvent::MouseButtonDblClick){
        return false;
    }

    return false;
}
