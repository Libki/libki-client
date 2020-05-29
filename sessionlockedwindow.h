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
 * along with Libki. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SESSIONLOCKEDWINDOW_H
#define SESSIONLOCKEDWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtDebug>
#include <QSettings>

#include "ui_sessionlockedwindow.h"

class SessionLockedWindow : public QMainWindow, public Ui::SessionLockedWindow {
  Q_OBJECT

public:

  SessionLockedWindow(QWidget *parent = 0, QString userUsername = "", QString userPassowrd = "");
  ~SessionLockedWindow();

  void closeEvent(QCloseEvent *event);

signals:

  void unlockSession();

public slots:

  void setAllowClose(bool);
//void displaySessionLockedWindow();

private slots:

  void attemptUnlock();

private:

  QIcon libkiIcon;

  QString username;
  QString password;

  bool allowClose;
  bool isHidden;

  void setupActions();
  void getSettings();
  void showMe();
  void setButtonsEnabled(bool);
};

#endif // SESSIONLOCKEDWINDOW_H
