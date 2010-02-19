/*
* Copyright 2010 Kyle M Hall <kyle.m.hall@gmail.com>
*
* This file is part of Koha Offline Circulation.
*
* Koha Offline Circulation is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Koha Offline Circulation is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Koha Offline Circulation.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>
#include <QtSql>

#include "mainwindow.h"
#include "borrowersearch.h"

LoginWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  TITLE = "Libki Kiosk System";
  VERSION = "1.0";

  setupUi(this);
  setupActions();

  mStatLabel = new QLabel;
  statusBar()->addPermanentWidget(mStatLabel);

  this->showMaximized();
  lineEditIssuesBorrowerCardnumber->setFocus();
}

LoginWindow::~LoginWindow() {
}

void LoginWindow::setupActions() {
}

void LoginWindow::getSettings() {
}

