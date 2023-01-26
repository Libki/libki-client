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

#include <QCryptographicHash>
#include <QMessageBox>
#include <QTextEdit>

#include "utils.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent) {
  qDebug("ENTER LoginWindow::LoginWindow");

  setAllowClose(false);

  setupUi(this);

  libkiIcon = QIcon(":images/libki_clock.png");
  this->setWindowIcon(libkiIcon);

  // Remove the maximize window button
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 ~Qt::WindowMaximizeButtonHint);

  // Remove the close window button
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 ~Qt::WindowSystemMenuHint);
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 Qt::WindowStaysOnTopHint);
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 Qt::X11BypassWindowManagerHint);
  setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                 Qt::FramelessWindowHint);

  setupActions();

  getSettings();

  reservedLabel->hide();
  serverAccessWarning->hide();
  internetAccessWarning->hide();

  clientNameLabel->setText(getClientName());

  handleBanners();

  showMe();

  qDebug("LEAVE LoginWindow::LoginWindow");
}

LoginWindow::~LoginWindow() {}

void LoginWindow::displayLoginWindow() {
  qDebug("ENTER LoginWindow::displayLoginWindow");

  showMe();

  qDebug("LEAVE LoginWindow::displayLoginWindow");
}

void LoginWindow::setupActions() {
  qDebug("ENTER LoginWindow::setupActions");

  connect(loginButton, SIGNAL(clicked()), this, SLOT(attemptLogin()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(resetLoginScreen()));

  qDebug("LEAVE LoginWindow::setupActions");
}

void LoginWindow::getSettings() {
  qDebug("ENTER LoginWindow::getSettings");

  /* Set Labels */
  QSettings settings;
  settings.setIniCodec("UTF-8");

  QString label = getLabel("username");
  if (!label.isEmpty()) {
    usernameLabel->setText(label);
  }

  label = getLabel("password");
  if (!label.isEmpty()) {
    passwordLabel->setText(label);
  }

  // Check for a local logo URL, then a server transmitted logo URL
  QString logoUrl = settings.value("images/logo").toString();
  int logoWidth = settings.value("images/logo_width").toInt();
  int logoHeight = settings.value("images/logo_height").toInt();

  if ( logoUrl.isEmpty() ) {
    logoUrl = settings.value("session/LogoURL").toString();
    logoWidth = settings.value("session/LogoWidth").toInt();
    logoHeight = settings.value("session/LogoHeight").toInt();
  }

  if (!logoUrl.isEmpty()) {
      qDebug() << "Logo URL: " << logoUrl;

      logo->hide();

      QPalette palette = logoWebView->palette();
      palette.setBrush(QPalette::Base, Qt::transparent);

      if (logoWidth) logoWebView->setMaximumWidth(logoWidth);
      if (logoHeight) logoWebView->setMaximumHeight(logoHeight);

      logoWebView->setEnabled(true);
      logoWebView->page()->setPalette(palette);
      logoWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
      logoWebView->load(QUrl(logoUrl));

      watermark->show();
  } else {
      logoWebView->hide();
      watermark->hide();
  }

  /* Hide Password Field */

  if (settings.value("node/no_passwords").toString().toInt()) {
    passwordLabel->hide();
    passwordField->hide();
  }

  qDebug("LEAVE LoginWindow::getSettings");
}

/* Protected Slots */
void LoginWindow::attemptLogin() {
  qDebug("ENTER LoginWindow::attemptLogin");

  QString username = usernameField->text();
  QByteArray password;
  password.append(passwordField->text());

  this->setButtonsEnabled(false);

  errorLabel->setText(tr("Please Wait..."));

  if (username.isEmpty()) {
    QSettings settings;
    settings.setIniCodec("UTF-8");
    QString md5FromIni = settings.value("node/password").toString();

    if (!md5FromIni.isEmpty()) {
      /* Check for shutdown password */
      QString passwordMd5 = QString(
          QCryptographicHash::hash(password, QCryptographicHash::Md5).toHex());
      qDebug() << "Password: " << password;
      qDebug() << "Hashed Password: " << passwordMd5;
      qDebug() << "Hash from INI file: " << md5FromIni;

      if (passwordMd5 == md5FromIni) {
        /* Shut it down */
        qDebug() << "Shutdown password matches, exiting.";

#ifdef Q_OS_WIN

        // If this is an MS Windows platform, use the keylocker programs to
        // limit mischief.
        QProcess::startDetached("c:/windows/explorer.exe");
        QProcess::startDetached("windows/on_login.exe");
#endif  // ifdef Q_OS_WIN
        exit(1);
      }
    }
  }

  QSettings settings;
  settings.setIniCodec("UTF-8");
  QString termsOfService = settings.value("session/TermsOfService").toString();
  QString termsOfServiceDetails = settings.value("session/TermsOfServiceDetails").toString();

  if (termsOfService.length() || termsOfServiceDetails.length() ) {
    QMessageBox msgBox;

    msgBox.setText(tr("Do you accept the terms of service?"));

    if ( termsOfService.length() ) {
        msgBox.setInformativeText(termsOfService);
    } else {
        msgBox.setInformativeText(tr("Terms of Service"));
    }

    qDebug() << "TERMS OF SERIVICE DETAILS: " << termsOfServiceDetails;
    if ( termsOfServiceDetails.length() ) {
        msgBox.setDetailedText(termsOfServiceDetails);
        if (Qt::mightBeRichText(termsOfServiceDetails)) {
          QTextEdit *detailedText = msgBox.findChild<QTextEdit *>();
          detailedText->setHtml(termsOfServiceDetails);
        }
    }

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    msgBox.setButtonText(QMessageBox::No, tr("No"));
    int ret = msgBox.exec();
    if (ret == QMessageBox::No) {
      resetLoginScreen();
      return;
    }
  }

  emit attemptLogin(username, password);

  qDebug("LEAVE LoginWindow::attemptLogin");
}

void LoginWindow::attemptLoginFailure(QString loginError) {
  qDebug() << QString("ENTER LoginWindow::attemptLoginFailure('%1')").arg(loginError);

  QString customErrorMessage = getLabel(loginError);
  if (!customErrorMessage.isEmpty()) {
    errorLabel->setText(customErrorMessage);
  } else if ((loginError == "BAD_LOGIN") || (loginError == "INVALID_USER") ||
             (loginError == "INVALID_PASSWORD")) {
    errorLabel->setText(tr("Login Failed: Username and password do not match"));
  } else if (loginError == "AGE_MISMATCH") {
    errorLabel->setText(
        tr("Login Failed: You are not the correct age to use this client"));
  } else if (loginError == "NO_TIME") {
    errorLabel->setText(tr("Login Failed: No time left"));
  } else if (loginError == "CLOSED") {
    errorLabel->setText(tr("Login Failed: This kiosk is closed for the day"));
  } else if (loginError == "ACCOUNT_IN_USE") {
    errorLabel->setText(tr("Login Failed: Account is currently in use"));
  } else if (loginError == "ACCOUNT_DISABLED") {
    errorLabel->setText(tr("Login Failed: Account is disabled"));
  } else if (loginError == "RESERVED_FOR_OTHER") {
    errorLabel->setText(
        tr("Login Failed: This kiosk is reserved for someone else"));
  } else if (loginError == "RESERVATION_REQUIRED") {
    errorLabel->setText(tr("Login Failed: Reservation required"));
  } else if (loginError == "FEE_LIMIT") {
    errorLabel->setText(
        tr("Login Failed: You have excessive outstanding fees"));
  } else if (loginError == "CHARGE_PRIVILEGES_DENIED") {
    errorLabel->setText(tr("Login Failed: Charge privileges denied"));
  } else if (loginError == "RENEWAL_PRIVILEGES_DENIED") {
    errorLabel->setText(tr("Login Failed: Renewal privileges denied"));
  } else if (loginError == "RECALL_PRIVILEGES_DENIED") {
    errorLabel->setText(tr("Login Failed: Recall privileges denied"));
  } else if (loginError == "HOLD_PRIVILEGES_DENIED") {
    errorLabel->setText(tr("Login Failed: Hold privileges denied"));
  } else if (loginError == "CARD_REPORTED_LOST") {
    errorLabel->setText(tr("Login Failed: Your card has been reported lost"));
  } else if (loginError == "TOO_MANY_ITEMS_CHARGED") {
    errorLabel->setText(
        tr("Login Failed: You have too many items charged to your account"));
  } else if (loginError == "TOO_MANY_ITEMS_OVERDUE") {
    errorLabel->setText(tr("Login Failed: You have too many items overdue"));
  } else if (loginError == "TOO_MANY_ITEMS_RENEWALS") {
    errorLabel->setText(
        tr("Login Failed: You have renewed items too many times"));
  } else if (loginError == "TOO_MANY_CLAIMS_OF_ITEMS_RETURNED") {
    errorLabel->setText(
        tr("Login Failed: You have claimed too many items as returned"));
  } else if (loginError == "TOO_MANY_ITEMS_LOST") {
    errorLabel->setText(tr("Login Failed: You have have lost too many items"));
  } else if (loginError == "EXCESSIVE_OUTSTANDING_FINES") {
    errorLabel->setText(
        tr("Login Failed: You have excessive outstanding fines"));
  } else if (loginError == "EXCESSIVE_OUTSTANDING_FEES") {
    errorLabel->setText(
        tr("Login Failed: You have excessive outstanding fees"));
  } else if (loginError == "RECALL_OVERDUE") {
    errorLabel->setText(
        tr("Login Failed: You have a recalled item which is overdue"));
  } else if (loginError == "TOO_MANY_ITEMS_BILLED") {
    errorLabel->setText(
        tr("Login Failed: You have been billed for too many items"));
  } else if (loginError == "INVALID_CLIENT") {
    errorLabel->setText(tr("Login Failed: Client not registered"));
  } else if (loginError == "CONNECTION_FAILURE") {
    errorLabel->setText(tr("Login Failed: Unable to connect to ILS"));
  } else if (loginError == "TOO_MANY_SESSIONS") {
    errorLabel->setText(
        tr("Login Failed: Too many concurrent sessions on this account"));
  } else if (loginError == "EXPIRED_CARD") {
    errorLabel->setText(
        tr("Login Failed: Expired Membership. Please inquire at the "
           "circulation desk."));
  } else {
    errorLabel->setText(tr("Login Failed: ") + loginError);
  }

  this->setButtonsEnabled(true);

  passwordField->clear();
  usernameField->setFocus();
  usernameField->selectAll();

  qDebug() << QString("LEAVE LoginWindow::attemptLoginFailure('%1')").arg(loginError);
}

void LoginWindow::attemptLoginSuccess(QString username, QString password,
                                      int minutes, int hold_items_count) {
  qDebug("ENTER LoginWindow::attemptLoginSuccess");
  resetLoginScreen();

  //QProcess process;
  QSettings settings;
  QString runOnLogin = settings.value("node/run_on_login").toString();
  if (!runOnLogin.isEmpty()) {
    QString passEnvToRunOnLogin =
        settings.value("node/pass_env_to_run_on_login").toString();
    if (!passEnvToRunOnLogin.isEmpty()) {
      QStringList envVarsToPass = passEnvToRunOnLogin.split(',');
      for (int i = 0; i < envVarsToPass.size(); ++i) {
        // On Qt 5.5 there is no way to pass environment variables to a detached
        // process, so we need to add the vars to the current environment with
        // qputenv
        if (envVarsToPass.at(i) == "username") {
          // qputenv needs a QByteArray as second parameter, hence the call to
          // toUtf8
          qputenv("LIBKI_USER_NAME", username.toUtf8());
        }
        if (envVarsToPass.at(i) == "password") {
          qputenv("LIBKI_USER_PASSWORD", password.toUtf8());
        }
        if (envVarsToPass.at(i) == "name") {
          qputenv("LIBKI_CLIENT_NAME",
                  settings.value("node/name").toString().toUtf8());
        }
        if (envVarsToPass.at(i) == "location") {
          qputenv("LIBKI_CLIENT_LOCATION",
                  settings.value("node/location").toString().toUtf8());
        }
      }
    }

    // Yes, these quotes around the command within string are required, IKR?
    QProcess::startDetached('"' + runOnLogin + '"');
  }

  emit loginSucceeded(username, password, minutes, hold_items_count);
  this->hide();

  isHidden = true;

  qDebug("LEAVE LoginWindow::attemptLoginSuccess");
}

void LoginWindow::resetLoginScreen() {
  qDebug("ENTER LoginWindow::resetLoginScreen");

  this->setButtonsEnabled(true);
  usernameField->clear();
  passwordField->clear();
  errorLabel->setText("");
  usernameField->setFocus();

  qDebug("LEAVE LoginWindow::resetLoginScreen");
}

void LoginWindow::showMe() {
  qDebug("ENTER LoginWindow::showMe");

  this->show();
  this->showMaximized();
  this->showFullScreen();

  this->raise();  // for MacOS
  this->activateWindow(); // for Windows

  /* FIXME: For some reason, setFixedSize is preventing the window from being
     fullscreen. Why? */

  // setFixedSize(width(), height()); // Prevent the window from being resized
  resetLoginScreen();

  isHidden = false;

  if (!reservedFor.isEmpty()) {
    handleReservationStatus(reservedFor);
  }

  qDebug("LEAVE LoginWindow::showMe");
}

void LoginWindow::setButtonsEnabled(bool b) {
  qDebug("ENTER LoginWindow::setButtonsEnabled");

  usernameField->setEnabled(b);
  passwordField->setEnabled(b);
  cancelButton->setEnabled(b);
  loginButton->setEnabled(b);

  qDebug("LEAVE LoginWindow::setButtonsEnabled");
}

void LoginWindow::setAllowClose(bool close) {
  qDebug("ENTER LoginWindow::setAllowClose");
  allowClose = close;
  qDebug("LEAVE LoginWindow::setAllowClose");
}

/* Reimplemented closeEvent to prevent application from being closed. */
void LoginWindow::closeEvent(QCloseEvent *event) {
  qDebug("ENTER LoginWindow::closeEvent");

  if (allowClose) {
    qDebug("Close Accepted");
    event->accept();
  } else {
    qDebug("Close Ignored");
    event->ignore();
  }

  qDebug("LEAVE LoginWindow::closeEvent");
}

void LoginWindow::handleReservationStatus(QString reserved_for) {
  qDebug("ENTER LoginWindow::handleReservationStatus");

  if (reserved_for.isEmpty()) {
    reservedLabel->hide();
  } else {
    QSettings settings;

    if (settings.value("session/ReservationShowUsername").toString() != "RSD" &&
        !reserved_for.isEmpty()) {
      reservedLabel->setText(tr("Reserved: ") + reserved_for);
    } else {
      reservedLabel->setText(tr("Reserved"));
    }
    reservedLabel->show();

    if (!isHidden) {
      emit displayingReservationMessage(reserved_for);
    }
  }

  reservedFor = reserved_for;

  qDebug("LEAVE LoginWindow::handleReservationStatus");
}

void LoginWindow::handleBanners() {
  qDebug("ENTER LoginWindow::handleBanners");

  QSettings settings;
  settings.setIniCodec("UTF-8");

  QPalette palette = bannerWebViewTop->palette();

  palette.setBrush(QPalette::Base, Qt::transparent);

  QString bannerTopUrl =
      "http://" + settings.value("session/BannerTopURL").toString();

  if (bannerTopUrl != "http://") {
    int bannerTopHeight = settings.value("session/BannerTopHeight").toInt();
    int bannerTopWidth = settings.value("session/BannerTopWidth").toInt();

    bannerWebViewTop->setEnabled(true);
    bannerWebViewTop->page()->setPalette(palette);
    bannerWebViewTop->setAttribute(Qt::WA_OpaquePaintEvent, false);

    if (bannerTopHeight) bannerWebViewTop->setMaximumHeight(bannerTopHeight);

    if (bannerTopWidth) bannerWebViewTop->setMaximumWidth(bannerTopWidth);
    bannerWebViewTop->load(QUrl(bannerTopUrl));
  }

  QString bannerBottomUrl =
      "http://" + settings.value("session/BannerBottomURL").toString();

  if (bannerBottomUrl != "http://") {
    int bannerBottomHeight =
        settings.value("session/BannerBottomHeight").toInt();
    int bannerBottomWidth = settings.value("session/BannerBottomWidth").toInt();

    bannerWebViewBottom->setEnabled(true);
    bannerWebViewBottom->page()->setPalette(palette);
    bannerWebViewBottom->setAttribute(Qt::WA_OpaquePaintEvent, false);

    if (bannerBottomHeight)
      bannerWebViewBottom->setMaximumHeight(bannerBottomHeight);

    if (bannerBottomWidth)
      bannerWebViewBottom->setMaximumWidth(bannerBottomWidth);
    bannerWebViewBottom->load(QUrl(bannerBottomUrl));
  }

  /* For when logo is specificed in server side setting */
  if (!settings.value("images/logo").toString().isEmpty()) {
    logo->hide();

    QPalette palette = logoWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);

    QString logoUrl = settings.value("images/logo").toString();
    qDebug() << "Logo URL: " << logoUrl;

    if (!logoUrl.isEmpty()) {
      int logoWidth = settings.value("images/logo_width").toInt();

      if (logoWidth) logoWebView->setMaximumWidth(logoWidth);

      int logoHeight = settings.value("images/logo_height").toInt();

      if (logoHeight) logoWebView->setMaximumHeight(logoHeight);

      logoWebView->setEnabled(true);
      logoWebView->page()->setPalette(palette);
      logoWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
      logoWebView->load(QUrl(logoUrl));
    }
  } else {
    logoWebView->hide();
  }

  qDebug("LEAVE LoginWindow::handleBanners");
}

void LoginWindow::disableLogin() {
  qDebug("ENTER LoginWindow::disableLogin");

  this->setButtonsEnabled(false);
  messageLabel->setVisible(false);
  errorLabel->setText(tr("This kiosk is out of order."));

  qDebug("LEAVE LoginWindow::disableLogin");
}

void LoginWindow::enableLogin() {
  qDebug("ENTER LoginWindow::enableLogin");

  this->resetLoginScreen();
  messageLabel->setVisible(true);

  qDebug("LEAVE LoginWindow::enableLogin");
}

void LoginWindow::showServerAccessWarning(QString message) {
  qDebug() << QString("ENTER LoginWindow::showServerAccessWarning(%1)").arg(message);

  if ( message.length() > 0 ) {
    serverAccessWarning->setText(tr("Error connecting to server. Verify Libki server is accessible from this network. Error Code: ")  + message );
    serverAccessWarning->show();
  } else {
    serverAccessWarning->hide();
  }

  qDebug("LEAVE LoginWindow::showServerAccessWarning");
}

void LoginWindow::showInternetAccessWarning(QString message) {
  qDebug() << QString("ENTER LoginWindow::showInternetAccessWarning(%1)").arg(message);

  if ( message.length() > 0 ) {
    internetAccessWarning->setText(tr("Error connecting to Internet: ") + message);
    internetAccessWarning->show();
  } else {
    internetAccessWarning->hide();
  }

  qDebug("LEAVE LoginWindow::showInternetAccessWarning");
}
