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

#include <QCryptographicHash>

#include "loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent) {
    qDebug("LoginWindow::LoginWindow");

    setAllowClose( false );

    setupUi(this);

    libkiIcon = QIcon(":images/libki_clock.png");
    this->setWindowIcon(libkiIcon);

    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint); // Remove the maximize window button
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowSystemMenuHint); // Remove the close window button
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::WindowStaysOnTopHint);
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::X11BypassWindowManagerHint);
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & Qt::FramelessWindowHint);

    setupActions();

    getSettings();

    reservedLabel->hide();

    handleBanners();

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
    QByteArray password;
    password.append(passwordField->text());

    this->setButtonsEnabled( false );

    errorLabel->setText( tr("Please Wait...") );

    if ( username.isEmpty() ) {
        QSettings settings;
        QString md5FromIni = settings.value("node/password").toString();

        if ( ! md5FromIni.isEmpty() ) {
            /* Check for shutdown password */
            QString passwordMd5 = QString(QCryptographicHash::hash(password,QCryptographicHash::Md5).toHex());
            qDebug() << "Password: " << password;
            qDebug() << "Hashed Password: " << passwordMd5;
            qDebug() << "Hash from INI file: " << md5FromIni;
            if ( passwordMd5 == md5FromIni ) {
                /* Shut it down */
                qDebug() << "Shutdown password matches, exiting.";

#ifdef Q_OS_WIN
                // If this is an MS Windows platform, use the keylocker programs to limit mischief.
                QProcess::startDetached("windows/on_login.exe");
#endif
                exit(1);
            }
        }
    }

    emit attemptLogin( username, password );
}

void LoginWindow::attemptLoginFailure( QString loginError ) {
    qDebug() << "LoginWindow::attemptLoginFailure(" + loginError + ")";

    if ( loginError == "BAD_LOGIN" || loginError == "INVALID_USER" || loginError == "INVALID_PASSWORD" ) {
        errorLabel->setText( tr("Login Failed: Username and password do not match") );
    } else if ( loginError == "NO_TIME" ) {
        errorLabel->setText( tr("Login Failed: No time left") );
    } else if ( loginError == "ACCOUNT_IN_USE" ) {
        errorLabel->setText( tr("Login Failed: Account is currently in use") );
    } else if ( loginError == "ACCOUNT_DISABLED" ) {
        errorLabel->setText( tr("Login Failed: Account is disabled") );
    } else if ( loginError == "RESERVED_FOR_OTHER" ) {
        errorLabel->setText( tr("Login Failed: This kiosk is reserved for someone else") );
    } else if ( loginError == "RESERVATION_REQUIRED" ) {
        errorLabel->setText( tr("Login Failed: Reservation required") );
    } else if ( loginError == "FEE_LIMIT" ) {
        errorLabel->setText( tr("Login Failed: You have excessive outstanding fees") );
    } else if ( loginError == "CHARGE_PRIVILEGES_DENIED" ) {
        errorLabel->setText( tr("Login Failed: Charge privileges denied") );
    } else if ( loginError == "RENEWAL_PRIVILEGES_DENIED" ) {
        errorLabel->setText( tr("Login Failed: Renewal privileges denied") );
    } else if ( loginError == "RECALL_PRIVILEGES_DENIED" ) {
        errorLabel->setText( tr("Login Failed: Recall privileges denied") );
    } else if ( loginError == "HOLD_PRIVILEGES_DENIED" ) {
        errorLabel->setText( tr("Login Failed: Hold privileges denied") );
    } else if ( loginError == "CARD_REPORTED_LOST" ) {
        errorLabel->setText( tr("Login Failed: Your card has been reported lost") );
    } else if ( loginError == "TOO_MANY_ITEMS_CHARGED" ) {
        errorLabel->setText( tr("Login Failed: You have too many items charged to your account") );
    } else if ( loginError == "TOO_MANY_ITEMS_OVERDUE" ) {
        errorLabel->setText( tr("Login Failed: You have too many items overdue") );
    } else if ( loginError == "TOO_MANY_ITEMS_RENEWALS" ) {
        errorLabel->setText( tr("Login Failed: You have renewed items too many times") );
    } else if ( loginError == "TOO_MANY_CLAIMS_OF_ITEMS_RETURNED" ) {
        errorLabel->setText( tr("Login Failed: You have claimed to many items as returned") );
    } else if ( loginError == "TOO_MANY_ITEMS_LOST" ) {
        errorLabel->setText( tr("Login Failed: You have have lost too many items") );
    } else if ( loginError == "EXCESSIVE_OUTSTANDING_FINES" ) {
        errorLabel->setText( tr("Login Failed: You have excessive outstanding fines") );
    } else if ( loginError == "EXCESSIVE_OUTSTANDING_FEES" ) {
        errorLabel->setText( tr("Login Failed: You have excessive outstanding fees") );
    } else if ( loginError == "RECALL_OVERDUE" ) {
        errorLabel->setText( tr("Login Failed: You have a recalled item which is overdue") );
    } else if ( loginError == "TOO_MANY_ITEMS_BILLED" ) {
        errorLabel->setText( tr("Login Failed: You have been billed for too many items") );
    } else if ( loginError == "INVALID_CLIENT" ) {
        errorLabel->setText( tr("Login Failed: Client not registered") );
    } else if ( loginError == "CONNECTION_FAILURE" ) {
        errorLabel->setText( tr("Login Failed: Unable to connect to ILS") );
    } else {
        errorLabel->setText( tr("Login Failed: ") + loginError );
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

void LoginWindow::handleBanners() {
    QSettings settings;

    QPalette palette = bannerWebViewTop->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);

    QString bannerTopUrl = "http://" + settings.value("session/BannerTopURL").toString();
    if ( ! bannerTopUrl.isEmpty() ) {
        int bannerTopHeight = settings.value("session/BannerTopHeight").toInt();
        int bannerTopWidth = settings.value("session/BannerTopWidth").toInt();

        bannerWebViewTop->setEnabled(true);
        bannerWebViewTop->page()->setPalette(palette);
        bannerWebViewTop->setAttribute(Qt::WA_OpaquePaintEvent, false);
        if ( bannerTopHeight ) { bannerWebViewTop->setMaximumHeight( bannerTopHeight ); }
        if ( bannerTopWidth ) { bannerWebViewTop->setMaximumWidth( bannerTopWidth ); }
        bannerWebViewTop->load(QUrl(bannerTopUrl));
    }

    QString bannerBottomUrl = "http://" + settings.value("session/BannerBottomURL").toString();
    if ( ! bannerBottomUrl.isEmpty() ) {
        int bannerBottomHeight = settings.value("session/BannerBottomHeight").toInt();
        int bannerBottomWidth = settings.value("session/BannerBottomWidth").toInt();

        bannerWebViewBottom->setEnabled(true);
        bannerWebViewBottom->page()->setPalette(palette);
        bannerWebViewBottom->setAttribute(Qt::WA_OpaquePaintEvent, false);
        if ( bannerBottomHeight ) { bannerWebViewBottom->setMaximumHeight( bannerBottomHeight ); }
        if ( bannerBottomWidth ) { bannerWebViewBottom->setMaximumWidth( bannerBottomWidth ); }
        bannerWebViewBottom->load(QUrl(bannerBottomUrl));
    }
}
