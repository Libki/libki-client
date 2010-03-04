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

#include "loginwindow.h"
#include "keypresseater.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint); // Remove the maximize window button
  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowSystemMenuHint); // Remove the close window button
  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::WindowStaysOnTopHint); 
  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::X11BypassWindowManagerHint);
  setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::FramelessWindowHint);

//  KeyPressEater* keyPressEater = new KeyPressEater(this);
//  this->installEventFilter(keyPressEater);

  setupActions();

  defaultMessage = messageLabel->text();
  errorLabel->setText("");

  this->showMaximized();
  this->showFullScreen();
  setFixedSize(width(), height()); // Prevent the window from being resized

  usernameField->setFocus();

}

LoginWindow::~LoginWindow() {
}

/* Reimplemented closeEvent to prevent application from being closed. */
void LoginWindow::closeEvent(QCloseEvent *event) {
  event->ignore();
}

void LoginWindow::setupActions() {

  connect(loginButton, SIGNAL(clicked()),
          this, SLOT(attemptLogin()));

  connect(cancelButton, SIGNAL(clicked()),
          this, SLOT(resetLoginScreen()));

}

void LoginWindow::getSettings() {
}

/* Protected Slots */
void LoginWindow::attemptLogin() {
  int loginError;

  QString username = usernameField->text();
  QString password = passwordField->text();

  int minutes = net.attemptLogin( username, password, loginError );

  if ( minutes ) {
    attemptLoginSuccess( username, password, minutes );
  } else {
	attemptLoginFailure( loginError );
  }
}

void LoginWindow::attemptLoginFailure( int loginError ) {
  if ( loginError == NetworkError::BAD_LOGIN ) {
    errorLabel->setText( tr("Login Failed: Username & Password Do Not Match") );
  } else if ( loginError == NetworkError::NO_TIME ) {
    errorLabel->setText( tr("Login Failed: No Time Left") );
  } else if ( loginError == NetworkError::SERVER_GONE ) {
    errorLabel->setText( tr("Login Failed: Unable To Connect To Server") );
  }

  passwordField->clear();
  usernameField->setFocus();
  usernameField->selectAll();
}

void LoginWindow::attemptLoginSuccess( QString& username, QString& password, int minutes ) {
  resetLoginScreen();

  qDebug() << "LoginWindow::attemptLoginSuccess :: Minutes: " << minutes;

  emit loginSucceeded( /*const QString& username, const QString& password, */ minutes );
  this->hide();
}

void LoginWindow::resetLoginScreen() {
  usernameField->clear();
  passwordField->clear();
  errorLabel->setText("");
  usernameField->setFocus();
}
