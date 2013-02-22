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

#include "loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent) {
    qDebug("LoginWindow::LoginWindow");

    setAllowClose( false );

    setupUi(this);

    libkiIcon = QIcon(":images/padlock.png");
    this->setWindowIcon(libkiIcon);

    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint); // Remove the maximize window button
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowSystemMenuHint); // Remove the close window button
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::WindowStaysOnTopHint);
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::X11BypassWindowManagerHint);
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::FramelessWindowHint);

    setupActions();

    getSettings();

    reservedLabel->hide();

    showMe();
}

LoginWindow::~LoginWindow() {
}

void LoginWindow::displayLoginWindow() {
    qDebug("LoginWindow::displayLoginWindow");
    showMe();
}

void LoginWindow::setupActions() {
    qDebug("LoginWindow::setupActions");

    connect(loginButton, SIGNAL(clicked()),
            this, SLOT(attemptLogin()));

    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(resetLoginScreen()));

}

void LoginWindow::getSettings() {

    /* Set Labels */
    QSettings settings;

    if ( ! settings.value("labels/username").toString().isEmpty() ) {
        usernameLabel->setText(settings.value("labels/username").toString() );
    }

    if ( ! settings.value("labels/password").toString().isEmpty() ) {
        passwordLabel->setText(settings.value("labels/password").toString() );
    }

    /* Hide Password Field */

    if ( settings.value("node/no_passwords").toString().toInt() ) {
        passwordLabel->hide();
        passwordField->hide();
    }
}

/* Protected Slots */
void LoginWindow::attemptLogin() {
    qDebug("LoginWindow::attemptLogin");
    QString username = usernameField->text();
    QString password = passwordField->text();

    this->setButtonsEnabled( false );

    errorLabel->setText( tr("Please Wait...") );

    emit attemptLogin( username, password );
}

void LoginWindow::attemptLoginFailure( QString loginError ) {
    qDebug() << "LoginWindow::attemptLoginFailure(" + loginError + ")";

    if ( loginError == "BAD_LOGIN" ) {
        errorLabel->setText( tr("Login Failed: Username & Password Do Not Match") );
    } else if ( loginError == "NO_TIME" ) {
        errorLabel->setText( tr("Login Failed: No Time Left") );
    } else if ( loginError == "ACCOUNT_IN_USE" ) {
        errorLabel->setText( tr("Login Failed: Account Is Currently In Use") );
    } else if ( loginError == "ACCOUNT_DISABLED" ) {
        errorLabel->setText( tr("Login Failed: Account Is Disabled") );
    } else if ( loginError == "RESERVED_FOR_OTHER" ) {
        errorLabel->setText( tr("Login Failed: This Kiosk Is Reserved For Someone Else") );
    } else if ( loginError == "RESERVATION_REQUIRED" ) {
        errorLabel->setText( tr("Login Failed: Reservation Required") );
    } else {
        errorLabel->setText( tr("Login Failed: Unable To Connect To Server") );
    }

    this->setButtonsEnabled( true );

    passwordField->clear();
    usernameField->setFocus();
    usernameField->selectAll();
}

void LoginWindow::attemptLoginSuccess( QString username, QString password, int minutes ) {
    qDebug("LoginWindow::attemptLoginSuccess");
    resetLoginScreen();

    emit loginSucceeded( username, password, minutes );
    this->hide();

    isHidden = true;
}

void LoginWindow::resetLoginScreen() {
    qDebug() << "LoginWindow::resetLoginScreen()";

    this->setButtonsEnabled( true );
    usernameField->clear();
    passwordField->clear();
    errorLabel->setText("");
    usernameField->setFocus();
}

void LoginWindow::showMe() {
    qDebug() << "LoginWindow::showMe()";

    this->show();
    this->showMaximized();
    this->showFullScreen();
    /* FIXME: For some reason, setFixedSize is preventing the window from being fullscreen. Why? */
    //setFixedSize(width(), height()); // Prevent the window from being resized
    resetLoginScreen();

    isHidden = false;

    if ( !reservedFor.isEmpty() ) {
        handleReservationStatus(reservedFor);
    }
}

void LoginWindow::setButtonsEnabled( bool b ){
    usernameField->setEnabled( b );
    passwordField->setEnabled( b );
    cancelButton->setEnabled( b );
    loginButton->setEnabled( b );
}

void LoginWindow::setAllowClose( bool close ){
    qDebug("LoginWindow::setAllowClose");
    allowClose = close;
}

/* Reimplemented closeEvent to prevent application from being closed. */
void LoginWindow::closeEvent(QCloseEvent *event) {
    if ( allowClose ){
        event->accept();
    } else {
        event->ignore();
    }
}

void LoginWindow::handleReservationStatus(QString reserved_for){
    qDebug("LoginWindow::handleReservationStatus");

    if ( reserved_for.isEmpty() ) {
        reservedLabel->hide();
    } else {
        QSettings settings;
        if ( settings.value("session/ReservationShowUsername").toBool() ) {
            reservedLabel->setText( tr("Reserved for ") + reserved_for );
        } else {
            reservedLabel->setText( tr("Reserved") );
        }
        reservedLabel->show();

        if ( !isHidden ) {
            emit displayingReservationMessage( reserved_for );
        }
    }

    reservedFor = reserved_for;
}
