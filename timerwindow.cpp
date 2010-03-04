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

#include <QtGui>
#include <QtDebug>

#include "timerwindow.h"

TimerWindow::TimerWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  setFixedSize(width(), height()); // Prevent the window from being resized
  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint); // Remove the maximize window button
  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowSystemMenuHint); // Remove the close window button

  setupActions();
  this->hide();
}

TimerWindow::~TimerWindow() {
}

/* Reimplemented closeEvent to prevent application from being closed. */
void TimerWindow::closeEvent(QCloseEvent *event) {
  event->ignore();
}

void TimerWindow::startTimer( /*const QString& username, const QString& password, */ int minutes ) {
  qDebug() << "TimerWindow::startTimer :: Minutes: " << minutes;

  this->show();

  minutesRemaining = minutesAtStart = minutes;

  updateClock();

  /* Start The Countdown */
  decrementMinutesTimer = new QTimer(this);
  connect(decrementMinutesTimer, SIGNAL(timeout()), this, SLOT(decrementMinutes()));
  decrementMinutesTimer->start( 1000 ); // Run once per minute ( 60000 milliseconds )
}

void TimerWindow::setupActions() {
}

void TimerWindow::getSettings() {
}

void TimerWindow::updateClock() {
  /* Convert minutes remaining into Hours::Minutes */
  int hours = minutesRemaining / 60;
  int minutes = minutesRemaining % 60;

  QString time = QString::number(hours) + ":" + QString::number(minutes).rightJustified(2, '0');

  lcdNumber->display( time );
  
  /* Update the progress bar */
  progressBar->setRange( 0, minutesAtStart );
  progressBar->setValue( minutesRemaining );
}

void TimerWindow::decrementMinutes() {
  minutesRemaining--;
  updateClock();
}

