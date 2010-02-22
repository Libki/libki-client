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
#include "ui_loginwindow.h"

#include "networkclient.h"

class LoginWindow : public QMainWindow, public Ui::LoginWindow {

  Q_OBJECT

  public:
    LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

  signals:
    void loginSucceeded( const QString& username, const QString& password, const int& minutes );

  protected:
    void setupActions();

    void getSettings();

  protected slots:
	void attemptLogin();
    void attemptLoginFailure( int loginError );
    void attemptLoginSuccess( QString username, QString password, int minutes );

	void resetLoginScreen();

  private:
	QString defaultMessage;
	NetworkClient net;

};

#endif // LOGINWINDOW_H
