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

#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtDebug>
#include <QSettings>

#include "ui_loginwindow.h"

#include "networkclient.h"

class LoginWindow : public QMainWindow, public Ui::LoginWindow {
  Q_OBJECT

public:

  LoginWindow(QWidget *parent = 0);
  ~LoginWindow();

  void closeEvent(QCloseEvent *event);

signals:

  void loginSucceeded(const QString& username,
                      const QString& password,
                      const int    & minutes,
                      const int    & hold_items_count);
  void attemptLogin(const QString& username,
                    const QString& password);
  void displayingReservationMessage(QString reserved_for);

public slots:

  void setAllowClose(bool);
  void displayLoginWindow();
  void attemptLogin();
  void attemptLoginFailure(QString loginError);
  void attemptLoginSuccess(QString username,
                           QString password,
                           int     minutes,
                           int     hold_items_count);
  void handleReservationStatus(QString reserved_for);
  void handleBanners();

private slots:

  void resetLoginScreen();

private:

  QIcon libkiIcon;

  QString reservedFor;

  bool allowClose;
  bool isHidden;

  void setupActions();
  void getSettings();
  void showMe();
  void setButtonsEnabled(bool);
};

#endif // LOGINWINDOW_H
