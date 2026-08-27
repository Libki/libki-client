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

#include "log.h"
#include "utils.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent) {
  ENTER_FUNC

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

  headerLabel->setText(getClientName());

  handleBanners();

  showMe();

  LEAVE_FUNC
}

LoginWindow::~LoginWindow() {}

void LoginWindow::displayLoginWindow() {
  ENTER_FUNC

  showMe();

  LEAVE_FUNC
}

void LoginWindow::setupActions() {
  ENTER_FUNC

  bool f = false;
  connect(loginButton, SIGNAL(clicked()), this, SLOT(attemptLogin()));
  connect(loginGuestButton, SIGNAL(clicked()), this, SLOT(attemptGuestLogin()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(resetLoginScreen()));

  LEAVE_FUNC
}

void LoginWindow::getSettings() {
  ENTER_FUNC

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

  if (logoUrl.isEmpty()) {
    logoUrl = settings.value("session/LogoURL").toString();
    logoWidth = settings.value("session/LogoWidth").toInt();
    logoHeight = settings.value("session/LogoHeight").toInt();
  }

  if (!logoUrl.isEmpty()) {
    LOG_SETTING("logoURL", logoUrl);

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

  if (settings.value("node/no_passwords").toString().toInt() ||
      settings.value("session/EnableClientPasswordlessMode")
          .toString()
          .toInt()) {
    passwordLabel->hide();
    passwordField->hide();
  }

  // TODO: Determine if we should actually log hiding the guest button
  // qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX: " <<
  // settings.value("session/EnableGuestSelfRegistration").toString();

  if (settings.value("session/EnableGuestSelfRegistration").toString() !=
      "enabled") {
    // TODO: Determine if we should actually log hiding the guest button
    // qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX: HIDE";
    loginGuestButton->hide();
  }

  LEAVE_FUNC
}

/* Protected Slots */
void LoginWindow::attemptGuestLogin() {
  ENTER_FUNC

  this->setButtonsEnabled(false);

  errorLabel->setText(tr("Please Wait..."));

  QSettings settings;
  settings.setIniCodec("UTF-8");
  QString termsOfService = settings.value("session/TermsOfService").toString();
  QString termsOfServiceDetails =
      settings.value("session/TermsOfServiceDetails").toString();

  if (termsOfService.length() || termsOfServiceDetails.length()) {
    QMessageBox msgBox;

    msgBox.setText(tr("Do you accept the terms of service?"));

    if (termsOfService.length()) {
      msgBox.setInformativeText(termsOfService);
    } else {
      msgBox.setInformativeText(tr("Terms of Service"));
    }

    // TODO: Determine if we should actually log the ToS
    // It may be too big to actually send in a single syslog message
    // qDebug() << "TERMS OF SERIVICE DETAILS: " << termsOfServiceDetails;
    if (termsOfServiceDetails.length()) {
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

  emit attemptLogin(QString(""), QString(""), true);

  LEAVE_FUNC
}

void LoginWindow::attemptLogin() {
  ENTER_FUNC

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

      // Let's not put plaintext passwords in the logs
      // qDebug() << "Password: " << password;

      bool allowed = passwordMd5 == md5FromIni;

      if (allowed) {
        /* Shut it down */
        log::info("Shutdown password matches, exiting.", BYPASS_ID,
                  SData::new_bypassattempt(passwordMd5, md5FromIni, allowed));

#ifdef Q_OS_WIN

        // If this is an MS Windows platform, use the keylocker programs to
        // limit mischief.
        QProcess::startDetached("c:/windows/explorer.exe");
        QProcess::startDetached("windows/on_login.exe");
#endif  // ifdef Q_OS_WIN
        exit(1);
      }

      log::info("Shutdown password didn't match.", BYPASS_ID,
                SData::new_bypassattempt(passwordMd5, md5FromIni, allowed));
    }
  }

  QSettings settings;
  settings.setIniCodec("UTF-8");
  QString termsOfService = settings.value("session/TermsOfService").toString();
  QString termsOfServiceDetails =
      settings.value("session/TermsOfServiceDetails").toString();

  if (termsOfService.length() || termsOfServiceDetails.length()) {
    QMessageBox msgBox;

    msgBox.setText(tr("Do you accept the terms of service?"));

    if (termsOfService.length()) {
      msgBox.setInformativeText(termsOfService);
    } else {
      msgBox.setInformativeText(tr("Terms of Service"));
    }

    // TODO: Determine if we should actually log the ToS
    // It may be too big to actually send in a single syslog message
    // qDebug() << "TERMS OF SERVICE DETAILS: " << termsOfServiceDetails;
    if (termsOfServiceDetails.length()) {
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

  emit attemptLogin(username, password, false);

  LEAVE_FUNC
}

void LoginWindow::attemptLoginFailure(QString loginError) {
  log::debug(
      QString("ENTER LoginWindow::attemptLoginFailure('%1')").arg(loginError),
      ENTERFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));

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

  log::debug(
      QString("LEAVE LoginWindow::attemptLoginFailure('%1')").arg(loginError),
      LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));
}

void LoginWindow::attemptLoginSuccess(QString username, QString password,
                                      int minutes, int hold_items_count) {
  ENTER_FUNC
  resetLoginScreen();

  // QProcess process;
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

  LEAVE_FUNC
}

void LoginWindow::resetLoginScreen() {
  ENTER_FUNC

  this->setButtonsEnabled(true);
  usernameField->clear();
  passwordField->clear();
  errorLabel->setText("");
  usernameField->setFocus();

  LEAVE_FUNC
}

void LoginWindow::showMe() {
  ENTER_FUNC

  this->show();
  this->showMaximized();
  this->showFullScreen();

  this->raise();           // for MacOS
  this->activateWindow();  // for Windows

  /* FIXME: For some reason, setFixedSize is preventing the window from being
     fullscreen. Why? */

  // setFixedSize(width(), height()); // Prevent the window from being resized
  resetLoginScreen();

  isHidden = false;

  if (!reservedFor.isEmpty()) {
    handleReservationStatus(reservedFor);
  }

  LEAVE_FUNC
}

void LoginWindow::setButtonsEnabled(bool b) {
  ENTER_FUNC

  usernameField->setEnabled(b);
  passwordField->setEnabled(b);
  cancelButton->setEnabled(b);
  loginButton->setEnabled(b);
  loginGuestButton->setEnabled(b);

  LEAVE_FUNC
}

void LoginWindow::setAllowClose(bool close) {
  ENTER_FUNC
  allowClose = close;
  LEAVE_FUNC
}

/* Reimplemented closeEvent to prevent application from being closed. */
void LoginWindow::closeEvent(QCloseEvent *event) {
  ENTER_FUNC

  if (allowClose) {
    log::debug("Close accepted.", CLOSEEVENT_ID, SData::new_nil());
    event->accept();
  } else {
    log::debug("Close ignored.", CLOSEEVENT_ID, SData::new_nil());
    event->ignore();
  }

  LEAVE_FUNC
}

void LoginWindow::handleReservationStatus(QString reserved_for) {
  ENTER_FUNC

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

  LEAVE_FUNC
}

void LoginWindow::handleBanners() {
  ENTER_FUNC

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
  if (!settings.value("session/LogoURL").toString().isEmpty()) {
    logo->hide();

    QPalette palette = logoWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);

    QString logoUrl = settings.value("session/LogoURL").toString();
    // TODO: Determine if we should actually log settings and how
    // qDebug() << "Logo URL: " << logoUrl;

    if (!logoUrl.isEmpty()) {
      int logoWidth = settings.value("session/LogoWidth").toInt();

      if (logoWidth) logoWebView->setMaximumWidth(logoWidth);

      int logoHeight = settings.value("session/LogoHeight").toInt();

      if (logoHeight) logoWebView->setMaximumHeight(logoHeight);

      logoWebView->setEnabled(true);
      logoWebView->page()->setPalette(palette);
      logoWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
      logoWebView->load(QUrl(logoUrl));
    }
  } else {
    logoWebView->hide();
  }

  /* okay, not strictly banners, but handling a server-side configuration
   * without needing a node setting */
  if (settings.value("session/EnableGuestSelfRegistration").toString() !=
      "enabled") {
    loginGuestButton->hide();
  } else {
    loginGuestButton->show();
  }

  LEAVE_FUNC
}

void LoginWindow::disableLogin() {
  ENTER_FUNC

  this->setButtonsEnabled(false);
  messageLabel->setVisible(false);
  errorLabel->setText(tr("This kiosk is out of order."));

  LEAVE_FUNC
}

void LoginWindow::enableLogin() {
  ENTER_FUNC

  this->resetLoginScreen();
  messageLabel->setVisible(true);

  LEAVE_FUNC
}

void LoginWindow::showServerAccessWarning(QString message) {
  log::debug(
      QString("ENTER LoginWindow::showServerAccessWarning('%1')").arg(message),
      ENTERFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));

  if (message.length() > 0) {
    serverAccessWarning->setText(
        tr("Error connecting to server. Verify Libki server is accessible from "
           "this network. Error Code: ") +
        message);
    serverAccessWarning->show();
  } else {
    serverAccessWarning->hide();
  }

  log::debug(
      QString("LEAVE LoginWindow::showServerAccessWarning('%1')").arg(message),
      LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));
}

void LoginWindow::showInternetAccessWarning(QString message) {
  log::debug(QString("LEAVE LoginWindow::showInternetAccessWarning('%1')")
                 .arg(message),
             ENTERFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));

  if (message.length() > 0) {
    internetAccessWarning->setText(tr("Error connecting to Internet: ") +
                                   message);
    internetAccessWarning->show();
  } else {
    internetAccessWarning->hide();
  }

  log::debug(QString("LEAVE LoginWindow::showInternetAccessWarning('%1')")
                 .arg(message),
             LEAVEFUNC_ID, SData::new_funcframe(__PRETTY_FUNCTION__));
}
