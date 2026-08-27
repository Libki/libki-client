#include "timesplash.h"

#include <QDebug>
#include <QEvent>

#include "log.h"
#include "timerwindow.h"

TimeSplash::TimeSplash(TimerWindow *tw, const QPixmap &pixmap,
                       Qt::WindowFlags f) {
  ENTER_FUNC

  timerwindow = tw;
  this->setPixmap(pixmap);
  this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

  this->installEventFilter(this);

  LEAVE_FUNC
}

bool TimeSplash::eventFilter(QObject *target, QEvent *event) {
  ENTER_FUNC

  Q_UNUSED(target)

  if (event->type() == QEvent::MouseButtonDblClick) {
    timerwindow->setWindowState(Qt::WindowMinimized);

    log::debug(QString("LEAVE %1 - Return false").arg(__PRETTY_FUNCTION__),
               LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));

    qDebug("LEAVE TimeSplash::eventFilter - Return false");
    return false;
  }

  if (event->type() == QEvent::MouseButtonPress) {
    timerwindow->setWindowState(Qt::WindowActive);
    timerwindow->show();
    timerwindow->raise();
    log::debug(QString("LEAVE %1 - Return true").arg(__PRETTY_FUNCTION__),
               LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));
    return true;
  }

  if ((event->type() == QEvent::MouseButtonRelease) ||
      (event->type() == QEvent::KeyPress) ||
      (event->type() == QEvent::KeyRelease)) {
    log::debug(QString("LEAVE %1 - Return true").arg(__PRETTY_FUNCTION__),
               LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));
    return true;
  }

  log::debug(QString("LEAVE %1 - Return false").arg(__PRETTY_FUNCTION__),
             LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));
  return false;
}
