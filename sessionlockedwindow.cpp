#include "sessionlockedwindow.h"

#include <QCryptographicHash>
#include <QMessageBox>

#include "log.h"
#include "utils.h"

SessionLockedWindow::SessionLockedWindow(QWidget *parent, QString userUsername,
                                         QString userPassword)
    : QMainWindow(parent) {
  ENTER_FUNC

  username = userUsername;
  password = userPassword;

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

  clientNameLabel->setText(getClientName());

  // handleBanners(); // Do we really want banners on the lock screen?

  this->show();
  this->showMaximized();
  this->showFullScreen();

  this->raise();           // for MacOS
  this->activateWindow();  // for Windows

  LEAVE_FUNC
}

SessionLockedWindow::~SessionLockedWindow() {}

void SessionLockedWindow::setAllowClose(bool close) {
  ENTER_FUNC

  allowClose = close;

  LEAVE_FUNC
}

/* Reimplemented closeEvent to prevent application from being closed. */
void SessionLockedWindow::closeEvent(QCloseEvent *event) {
  ENTER_FUNC

  if (allowClose) {
    event->accept();
  } else {
    event->ignore();
  }

  LEAVE_FUNC
}

void SessionLockedWindow::getSettings() {
  ENTER_FUNC

  /* Set Labels */
  QSettings settings;
  settings.setIniCodec("UTF-8");

  QString label = getLabel("password");
  if (!label.isEmpty()) {
    passwordLabel->setText(label);
  }

  if (!settings.value("images/logo").toString().isEmpty()) {
    logo->hide();

    QPalette palette = logoWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);

    QString logoUrl = settings.value("images/logo").toString();
    LOG_SETTING("images/logo", logoUrl);

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

  LEAVE_FUNC
}

void SessionLockedWindow::attemptUnlock() {
  ENTER_FUNC

  QString passwordEntered = passwordField->text();

  if (passwordEntered == password) {
    passwordField->clear();
    emit unlockSession();
  } else {
    messageLabel->setText(tr("Incorrect Password"));
    passwordField->clear();
  }

  LEAVE_FUNC
}

void SessionLockedWindow::setupActions() {
  ENTER_FUNC

  connect(resumeButton, SIGNAL(clicked()), this, SLOT(attemptUnlock()));

  //  connect(cancelButton, SIGNAL(clicked()),
  //          this, SLOT(resetSessionLockedScreen()));

  LEAVE_FUNC
}
