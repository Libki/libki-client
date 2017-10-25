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

#include <QtWidgets/qdesktopwidget.h>
#include "timerwindow.h"

TimerWindow::TimerWindow(QWidget *parent) : QMainWindow(parent) {
  qDebug("TimerWindow::TimerWindow");

  setAllowClose(false);

  setupUi(this);

  libkiIcon = QIcon(":images/libki_clock.png");

  this->setWindowIcon(libkiIcon);

  // Prevent the window from being resized
  setFixedSize(width(), height());

  // Remove the maximize window button
  setWindowFlags(
    (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  // Remove the close window button
  setWindowFlags(
    (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowSystemMenuHint);

  setupActions();
  setupTrayIcon();

  trayIconPopupTimer = new QTimer(this);
  connect(trayIconPopupTimer, SIGNAL(timeout()), this,
          SLOT(showSystemTrayIconTimeLeftMessage()));


  // Disable unused features
  pauseButton->hide();

  this->move(
    QApplication::desktop()->screen()->rect().center() - this->rect().center());

  this->hide();
}

TimerWindow::~TimerWindow() {}

void TimerWindow::startTimer(const QString&,
                             const QString&,
                             int minutes,
                             int hold_items_count) {
  qDebug("TimerWindow::startTimer");

  trayIconPopupTimer->start(1000 * 60); // Fire once a minute

  this->show();
  trayIcon->show();

  minutesRemaining = minutesAtStart = minutes;
  updateClock();

  QSettings settings;

  QString waiting_holds_message =
    "You have one or more items on hold waiting for pickup. Please contact a librarian for more details";

  if (!settings.value("labels/waiting_holds").toString().isEmpty()) {
    waiting_holds_message = settings.value("labels/waiting_holds").toString();
  }

  if (hold_items_count > 0) {
    this->showMessage(waiting_holds_message);
  }
}

void TimerWindow::stopTimer() {
  qDebug("TimerWindow::stopTimer");
  trayIconPopupTimer->stop();
  trayIcon->hide();
  this->hide();

  emit timerStopped();
}

void TimerWindow::updateClock() {
  qDebug("TimerWindow::updateClock");

  /* Convert minutes remaining into Hours::Minutes */
  int hours   = minutesRemaining / 60;
  int minutes = minutesRemaining % 60;

  QString time = QString::number(hours) + ":" +
                 QString::number(minutes).rightJustified(2, '0');

  lcdNumber->display(time);

  /* Update the progress bar */
  if (minutesRemaining > minutesAtStart) minutesAtStart = minutesRemaining;
  progressBar->setRange(0, minutesAtStart);
  progressBar->setValue(minutesRemaining);

  trayIcon->setToolTip(QString::number(minutesRemaining) + " " + tr(
                         "Minutes Left"));

  this->setWindowTitle("Libki " + time);
}

void TimerWindow::updateTimeLeft(int minutes) {
  qDebug() << "TimerWindow::updateTimeLeft( " << minutes << " )";

  minutesRemaining = minutes;
  updateClock();

  if (minutesRemaining <= 0) {
    emit requestLogout();
  }
}

void TimerWindow::doLogoutDialog() {
  QMessageBox msgBox;

  msgBox.setWindowIcon(libkiIcon);
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setText(tr("Log Out?"));
  msgBox.setInformativeText(tr("Are you sure you want to log out?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Yes);
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
}

void TimerWindow::setupActions() {
  qDebug("TimerWindow::setupActions");
  connect(logoutButton, SIGNAL(clicked()), this, SLOT(doLogoutDialog()));
}

void TimerWindow::setupTrayIcon() {
  qDebug("TimerWindow::setupTrayIcon()");

  trayIconMenu = new QMenu(this);

  QAction *logoutAction =
    new QAction(QIcon(":icons/system_log_out.png"), tr("Log Out"), this);
  connect(logoutAction, SIGNAL(triggered()), this, SLOT(doLogoutDialog()));
  trayIconMenu->addAction(logoutAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->setIcon(libkiIcon);

  connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(restoreTimerWindow()));

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void TimerWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
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
}

void TimerWindow::restoreTimerWindow() {
  qDebug("TimerWindow::restoreTimerWindow");

  // TODO: TimerWindow will not come to front if behind other windows. Needed
  // for showMessage().
  // this->setWindowState(Qt::WindowStates(Qt::WindowActive)); // Doesn't help.
  this->activateWindow();
  this->raise();
  this->showNormal();
}

void TimerWindow::showSystemTrayIconTimeLeftMessage() {
  qDebug("showSystemTrayIconTimeLeftMessage");

  QString title   = tr("Time Remaining");
  QString message = QString::number(minutesRemaining) + " " + tr("Minutes Left");

  if (!(minutesRemaining % 5)) {
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 1000);
  } else if (minutesRemaining <= 5) {
    trayIcon->showMessage(title, message, QSystemTrayIcon::Warning, 1000);
  }
}

void TimerWindow::showMessage(QString message) {
  qDebug() << "TimerWindow::showMessage(" << message << ")";

  QMessageBox msgBox;
  msgBox.setWindowIcon(libkiIcon);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(tr("You have a message"));
  msgBox.setInformativeText(message);

  this->restoreTimerWindow();
  msgBox.exec();
}

void TimerWindow::getSettings() {}

void TimerWindow::setAllowClose(bool close) {
  qDebug("TimerWindow::setAllowClose");
  allowClose = close;
}

/* Reimplemented closeEvent to prevent application from being closed. */
void TimerWindow::closeEvent(QCloseEvent *event) {
  if (allowClose) {
    event->accept();
  } else {
    event->ignore();
  }
}
