#include <QEvent>
#include <QDebug>

#include "timesplash.h"
#include "timerwindow.h"


TimeSplash::TimeSplash(TimerWindow* tw, const QPixmap &pixmap, Qt::WindowFlags f)
{
    timerwindow = tw;
    this->setPixmap(pixmap);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    this->installEventFilter(this);
}

bool TimeSplash::eventFilter(QObject *target, QEvent *event)
{
    Q_UNUSED(target)


    if( event->type() == QEvent::MouseButtonDblClick ){
        timerwindow->setWindowState(Qt::WindowMinimized);
        return false;
    }

    if( event->type() == QEvent::MouseButtonPress ){
        timerwindow->setWindowState(Qt::WindowActive);
        timerwindow->show();
        timerwindow->raise();
        return true;
    }

    if(
       (event->type() == QEvent::MouseButtonRelease) ||
       (event->type() == QEvent::KeyPress) ||
       (event->type() == QEvent::KeyRelease)
    ) {
        return true;
    }

    return false;
}
