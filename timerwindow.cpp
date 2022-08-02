/*
 * Copyright 2010 Kyle M Hall <kyle.m.hall@gmail.com>
 *
 * This file is part of Libki.
 *
 * Libki is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Libki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Libki.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "timerwindow.h"

#include <QtWidgets/qdesktopwidget.h>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QScreen>

#include "sessionlockedwindow.h"
#include "utils.h"
#include "timesplash.h"

#define INACTIVITY_CHECK_INTERVAL 10

TimerWindow::TimerWindow(QWidget *parent) : QMainWindow(parent) {
  qDebug("ENTER TimerWindow::TimerWindow");

  sessionLockedWindow = Q_NULLPTR;

  setAllowClose(false);

  setupUi(this);

  libkiIcon = QIcon(":/images/images/libki_clock.png");

  this->setWindowIcon(libkiIcon);

  // Prevent the window from being resized
  setFixedSize(width(), height());

  // Remove the maximize window button
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 ~Qt::WindowMaximizeButtonHint);

  // Remove the close window button
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 ~Qt::WindowSystemMenuHint);

  setupActions();
  setupTrayIcon();

  // Set up the timer splash
  QPixmap pixmap(":/images/images/time_splash_background.png");
  timeSplash = new TimeSplash( this, pixmap, Qt::WindowStaysOnTopHint );

  trayIconPopupTimer = new QTimer(this);
  connect(trayIconPopupTimer, SIGNAL(timeout()), this,
          SLOT(showSystemTrayIconTimeLeftMessage()));

  inactivityTimer = new QTimer(this);
  connect(inactivityTimer, SIGNAL(timeout()), this, SLOT(checkForInactivity()));

  this->move(QApplication::desktop()->screen()->rect().center() -
             this->rect().center());

  this->hide();

  qDebug("LEAVE TimerWindow::TimerWindow");
}

TimerWindow::~TimerWindow() {}

void TimerWindow::startTimer(QString newUsername, QString newPassword,
                             int minutes, int hold_items_count) {
  qDebug("ENTER TimerWindow::startTimer");

  username = newUsername;
  password = newPassword;

  trayIconPopupTimer->start(1000 * 60);  // Fire once a minute

  secondsSinceLastActivity = 0;
  inactivityTimer->start(1000 * INACTIVITY_CHECK_INTERVAL);

  this->show();
  trayIcon->show();

  minutesRemaining = minutesAtStart = minutes;
  updateClock();

  QSettings settings;
  settings.setIniCodec("UTF-8");

  if (settings.value("session/EnableClientSessionLocking").toBool()) {
    connect(lockSessionButton, SIGNAL(clicked(bool)), this,
            SLOT(lockSession()));
  } else {
    lockSessionButton->hide();
  }

  QString waiting_holds_message =
      tr("You have one or more items on hold waiting for pickup. Please "
         "contact a librarian for more details");

  QString label = getLabel("waiting_holds");
  if (!label.isEmpty()) {
    waiting_holds_message = label;
  }

  if (hold_items_count > 0) {
    this->showMessage(waiting_holds_message);
  }

  qDebug("LEAVE TimerWindow::startTimer");
}

void TimerWindow::stopTimer() {
  qDebug("ENTER TimerWindow::stopTimer");

  inactivityTimer->stop();

  trayIconPopupTimer->stop();
  trayIcon->hide();
  this->hide();

  if (sessionLockedWindow) {
    sessionLockedWindow->hide();
    delete sessionLockedWindow;
    sessionLockedWindow = Q_NULLPTR;
  }

  emit timerStopped();

  qDebug("LEAVE TimerWindow::stopTimer");
}

void TimerWindow::updateClock() {
  qDebug("ENTER TimerWindow::updateClock");

  QSettings settings;
  settings.setIniCodec("UTF-8");

  /* Convert minutes remaining into Hours::Minutes */
  int hours = minutesRemaining / 60;
  int minutes = minutesRemaining % 60;

  QString time = QString::number(hours) + ":" +
                 QString::number(minutes).rightJustified(2, '0');

  lcdNumber->display(time);

  /* Update the progress bar */
  if (minutesRemaining > minutesAtStart) minutesAtStart = minutesRemaining;
  progressBar->setRange(0, minutesAtStart);
  progressBar->setValue(minutesRemaining);

  QString minutesString = QString::number(minutesRemaining);

  trayIcon->setToolTip( minutesString + " " +
                       tr("Minutes Left"));

  this->setWindowTitle("Libki " + time);

  if ( settings.value("node/showTimeRemainingInTray").toInt() == 1 ) {
      // Update the system tray icon
      QPixmap libkiIcon = QPixmap(":/images/images/tray.png");
      QPainter painter(&libkiIcon);
      QFont font = painter.font() ;
      font.setBold(true);
      if ( minutesRemaining < 10 ) {
        font.setPointSize(14);
      } else if ( minutesRemaining < 100 ) {
        font.setPointSize(11);
      } else {
        font.setPointSize(9);
      }
      painter.setFont(font);

      if ( this->swapColors ) {
          painter.setPen(QColor(Qt::black));
      } else {
          painter.setPen(QColor(Qt::white));
      }
      this->swapColors = !this->swapColors;
      painter.drawText(libkiIcon.rect(), Qt::AlignCenter, minutesString);

      trayIcon->setIcon(libkiIcon);
  }

  bool showSplash = settings.value("node/showTimeRemainingInSplash").toInt() == 1;
  if ( sessionLockedWindow && sessionLockedWindow->isVisible() ) showSplash = false;
  if ( showSplash ) {
      // Update the time splash
      QScreen* screen = QGuiApplication::screens()[0];
      QRect screenrect = screen->availableGeometry();

      QPixmap pixmap(":/images/images/time_splash_background.png");

      QPainter painter(&pixmap);
      QFont font = painter.font() ;
      font.setBold(true);
      font.setPointSize(30);
      painter.setFont(font);
      painter.drawText(pixmap.rect(), Qt::AlignCenter, minutesString);

      timeSplash->setPixmap(pixmap);
      timeSplash->move(screenrect.right() -  timeSplash->width(), screenrect.bottom() - timeSplash->height());
      timeSplash->show();
      timeSplash->raise(); // Some X11 window managers do not support the "stays on top" flag. A solution is to set up a timer that periodically calls raise() on the splash screen to simulate the "stays on top" effect.

      QCoreApplication::processEvents();
  } else {
      timeSplash->hide();
  }

  qDebug("LEAVE TimerWindow::updateClock");
}

void TimerWindow::updateTimeLeft(int minutes) {
  qDebug() << QString("ENTER TimerWindow::updateTimeLeft(%1)").arg(minutes);

  minutesRemaining = minutes;
  updateClock();

  if (minutesRemaining <= 0) {
    emit requestLogout();
  }

  qDebug() << QString("LEAVE TimerWindow::updateTimeLeft(%1)").arg(minutes);
}

void TimerWindow::doLogoutDialog() {
  qDebug("ENTER TimerWindow::doLogoutDialog");

  QMessageBox msgBox;

  msgBox.setWindowIcon(libkiIcon);
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setText(tr("Log Out?"));
  msgBox.setInformativeText(tr("Are you sure you want to log out?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
  msgBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));
  int ret = msgBox.exec();

  switch (ret) {
    case QMessageBox::Yes:
      emit requestLogout();
      break;

    case QMessageBox::Cancel:
      break;

    default:
      break;
  }

  qDebug("LEAVE TimerWindow::doLogoutDialog");
}

void TimerWindow::setupActions() {
  qDebug("ENTER TimerWindow::setupActions");

  connect(logoutButton, SIGNAL(clicked()), this, SLOT(doLogoutDialog()));

  qDebug("LEAVE TimerWindow::setupActions");
}

void TimerWindow::setupTrayIcon() {
  qDebug("ENTER TimerWindow::setupTrayIcon");

  trayIconMenu = new QMenu(this);

  QAction *logoutAction =
      new QAction(QIcon(":icons/system_log_out.png"), tr("Log Out"), this);
  connect(logoutAction, SIGNAL(triggered()), this, SLOT(doLogoutDialog()));
  trayIconMenu->addAction(logoutAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);

  QPixmap libkiIcon = QPixmap(":/icons/images/icons/libki.ico");
  trayIcon->setIcon(libkiIcon);

  connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(restoreTimerWindow()));

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
          SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  qDebug("LEAVE TimerWindow::setupTrayIcon");
}

void TimerWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  qDebug("ENTER TimerWindow::iconActivated");

  switch (reason) {
    case QSystemTrayIcon::Trigger:
      break;

    case QSystemTrayIcon::DoubleClick:
      restoreTimerWindow();
      break;

    case QSystemTrayIcon::MiddleClick:
      break;

    case QSystemTrayIcon::Context:
      qDebug("CONTEXT MENU");
      trayIcon->contextMenu()->showNormal();
      break;

    default:
      break;
  }

  qDebug("LEAVE TimerWindow::iconActivated");
}

void TimerWindow::restoreTimerWindow() {
  qDebug("ENTER TimerWindow::restoreTimerWindow");

  // TODO: TimerWindow will not come to front if behind other windows. Needed
  // for showMessage().
  // this->setWindowState(Qt::WindowStates(Qt::WindowActive)); // Doesn't help.
  this->activateWindow();
  this->raise();
  this->showNormal();

  qDebug("LEAVE TimerWindow::restoreTimerWindow");
}

void TimerWindow::showSystemTrayIconTimeLeftMessage() {
  qDebug("ENTER TimerWindow::showSystemTrayIconTimeLeftMessage");

  QString title = tr("Time Remaining");
  QString message =
      QString::number(minutesRemaining) + " " + tr("Minutes Left");

  if (!(minutesRemaining % 5)) {
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 1000);
  } else if (minutesRemaining <= 5) {
    trayIcon->showMessage(title, message, QSystemTrayIcon::Warning, 1000);
  }

  qDebug("LEAVE TimerWindow::showSystemTrayIconTimeLeftMessage");
}

void TimerWindow::checkForInactivity() {
  qDebug("ENTER TimerWindow::checkForInactivity");

  // TODO: keep one object level instance of settings for each of TimerWindow
  // and NetworkClient
  QSettings settings;
  settings.setIniCodec("UTF-8");

  int inactivityLogout = 0;
  if (!settings.value("node/inactivityLogout").toString().isEmpty()) {
    inactivityLogout = settings.value("node/inactivityLogout").toInt();
  } else if (!settings.value("session/inactivityLogout").toString().isEmpty()) {
    inactivityLogout = settings.value("session/inactivityLogout").toInt();
  }
  qDebug() << "INACTIVIY LOGOUT: " << inactivityLogout;

  int inactivityWarning = 5;
  if (!settings.value("node/inactivityWarning").toString().isEmpty()) {
    inactivityWarning = settings.value("node/inactivityWarning").toInt();
  } else if (!settings.value("session/inactivityWarning")
                  .toString()
                  .isEmpty()) {
    inactivityWarning = settings.value("session/inactivityWarning").toInt();
  }

  qDebug() << "INACTIVIY WARNING: " << inactivityWarning;

  if (inactivityLogout > 0) {
    QPoint pos = QCursor::pos();
    int x = pos.x();
    int y = pos.y();

    // TODO: Implement ranges to account for mouse wobble?
    if (prevMousePosX == x && prevMousePosY == y) {
      secondsSinceLastActivity += INACTIVITY_CHECK_INTERVAL;
      qDebug() << "No activity detected. Seconds since last activity: "
               << secondsSinceLastActivity;
    } else {
      secondsSinceLastActivity = 0;
      qDebug() << "Activity detected. Seconds since last activity: 0";
    }

    if (secondsSinceLastActivity / 60 >= inactivityWarning) {
      QString title = tr("Inactivity detected");
      QString message = tr("Please confirm you are still using this computer.");

      /* This would be useful if we could bring the timer window to the
       * forefront, but there appears to be no way to do that QMessageBox*
       * msgBox = new QMessageBox( this ); msgBox->setAttribute(
       * Qt::WA_DeleteOnClose ); //makes sure the msgbox is deleted
       * automatically when closed msgBox->setStandardButtons( QMessageBox::Ok
       * ); msgBox->setWindowTitle( title ); msgBox->setText( message );
       * msgBox->setWindowState( (windowState() & ~Qt::WindowMinimized) |
       * Qt::WindowActive); msgBox->raise(); msgBox->activateWindow();
       * msgBox->open();
       */

      QCoreApplication::processEvents();  // Should clear out any previous
                                          // system tray message
      trayIcon->showMessage(title, message, QSystemTrayIcon::Critical, 100000);
    }

    if (secondsSinceLastActivity / 60 >= inactivityLogout) {
      emit requestLogout();
    }

    prevMousePosX = x;
    prevMousePosY = y;
  }

  qDebug("LEAVE TimerWindow::checkForInactivity");
}

void TimerWindow::showMessage(QString message) {
  qDebug() << QString("ENTER TimerWindow::showMessage(%1)").arg(message);

  QMessageBox msgBox;
  msgBox.setWindowIcon(libkiIcon);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(tr("You have a message"));
  msgBox.setInformativeText(message);

  this->restoreTimerWindow();
  msgBox.exec();

  qDebug() << QString("LEAVE TimerWindow::showMessage(%1)").arg(message);
}

void TimerWindow::lockSession() {
  qDebug("ENTER TimerWindow::lockSession()");

  sessionLockedWindow = new SessionLockedWindow(0, username, password);
  connect(sessionLockedWindow, SIGNAL(unlockSession()), this,
          SLOT(unlockSession()));

  QProcess::startDetached("windows/on_startup.exe");
  this->hide();
  timeSplash->hide();
  sessionLockedWindow->show();

  qDebug("LEAVE TimerWindow::lockSession()");
}

void TimerWindow::unlockSession() {
  qDebug("ENTER TimerWindow::unlockSession");

  QProcess::startDetached("windows/on_login.exe");

  sessionLockedWindow->hide();
  this->show();
  delete sessionLockedWindow;
  sessionLockedWindow = Q_NULLPTR;

  qDebug("LEAVE TimerWindow::unlockSession");
}

void TimerWindow::getSettings() {}

void TimerWindow::setAllowClose(bool close) {
  qDebug("ENTER TimerWindow::setAllowClose");

  allowClose = close;

  qDebug("LEAVE TimerWindow::setAllowClose");
}

/* Reimplemented closeEvent to prevent application from being closed. */
void TimerWindow::closeEvent(QCloseEvent *event) {
  qDebug("ENTER TimerWindow::closeEvent");

  if (allowClose) {
    event->accept();
  } else {
    event->ignore();
  }

  qDebug("LEAVE TimerWindow::closeEvent");
}
