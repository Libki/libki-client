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

#include "timerwindow.h"

TimerWindow::TimerWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);
  setupActions();
  this->hide();
}

LoginWindow::~LoginWindow() {
}

void LoginWindow::startTimer( QString username, QString password, int minutes ) {
  this->show();
}

void LoginWindow::setupActions() {

}

void LoginWindow::getSettings() {
}

