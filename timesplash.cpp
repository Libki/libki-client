#include <QEvent>
#include <QDebug>

#include "timesplash.h"
#include "timerwindow.h"


TimeSplash::TimeSplash(TimerWindow* tw, const QPixmap &pixmap, Qt::WindowFlags f)
{
    qDebug("ENTER TimeSplash::TimeSplash");

    timerwindow = tw;
    this->setPixmap(pixmap);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    this->installEventFilter(this);

    qDebug("LEAVE TimeSplash::TimeSplash");
}

bool TimeSplash::eventFilter(QObject *target, QEvent *event)
{
    qDebug("ENTER TimeSplash::eventFilter");

    Q_UNUSED(target)

    if( event->type() == QEvent::MouseButtonDblClick ){
        timerwindow->setWindowState(Qt::WindowMinimized);

        qDebug("LEAVE TimeSplash::eventFilter - Return false");
        return false;
    }

    if( event->type() == QEvent::MouseButtonPress ){
        timerwindow->setWindowState(Qt::WindowActive);
        timerwindow->show();
        timerwindow->raise();
        qDebug("LEAVE TimeSplash::eventFilter - Return true");
        return true;
    }

    if(
       (event->type() == QEvent::MouseButtonRelease) ||
       (event->type() == QEvent::KeyPress) ||
       (event->type() == QEvent::KeyRelease)
    ) {
        qDebug("LEAVE TimeSplash::eventFilter - Return true");
        return true;
    }

    qDebug("LEAVE TimeSplash::eventFilter - Return false");
    return false;
}
