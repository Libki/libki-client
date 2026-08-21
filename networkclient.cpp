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

#include "networkclient.h"

#include <QDir>
#include <QHttpMultiPart>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QSslError>
#include <QUdpSocket>

#include "log.h"
#include "printprotocol.h"
#include "printsubmissionserver.h"
#include "utils.h"

#define VERSION "2.4.0"

NetworkClient::NetworkClient(QApplication *app) : QObject() {
  ENTER_FUNC

  this->app = app;

  log::debug(NIL_ID, QString("SSL version used for build: %1")
                         .arg(QSslSocket::sslLibraryBuildVersionString()));
  log::debug(NIL_ID, QString("SSL version used for runtime: %1")
                         .arg(QSslSocket::sslLibraryVersionNumber()));

  fileCounter = 0;

  QSettings settings;
  settings.setIniCodec("UTF-8");

  nodeName = getClientName();

  nodeLocation = settings.value("node/location").toString();
  LOG_SETTING("node/location", nodeLocation);
  nodeType = settings.value("node/type").toString();
  LOG_SETTING("node/type", nodeType);
  nodeAgeLimit = settings.value("node/age_limit").toString();
  LOG_SETTING("node/age_limt", nodeAgeLimit);

  QString action = settings.value("node/logoutAction").toString();

  if (action == "logout") {
    actionOnLogout = LogoutAction::Logout;
  } else if (action == "reboot") {
    actionOnLogout = LogoutAction::Reboot;
  } else {
    actionOnLogout = LogoutAction::NoAction;
  }

  clientStatus = "online";

  QString server_host = settings.value("server/host").toString();
  LOG_SETTING("server/host", server_host);
  serviceURL.setHost(server_host);

  int server_port = settings.value("server/port").toInt();
  LOG_SETTING("server/port", QString(server_port));
  serviceURL.setPort(server_port);

  QString server_scheme = settings.value("server/scheme").toString();
  LOG_SETTING("server/scheme", server_scheme);
  serviceURL.setScheme(server_scheme);

  serviceURL.setPath("/api/client/v1_0");

  customHeaderName = settings.value("server/customHeaderName").toString();
  customHeaderValue = settings.value("server/customHeaderValue").toString();

  nodeIPAddress = getIPv4Address();
  nodeMACAddress = getMACAddress();
  nodeHostname = getHostname();

  urlQuery.addQueryItem("node", nodeName);
  urlQuery.addQueryItem("location", nodeLocation);
  urlQuery.addQueryItem("type", nodeType);
  urlQuery.addQueryItem("ipaddress", nodeIPAddress);
  urlQuery.addQueryItem("macaddress", nodeMACAddress);
  urlQuery.addQueryItem("hostname", nodeHostname);

  registerNode();
  registerNodeTimer = new QTimer(this);
  connect(registerNodeTimer, SIGNAL(timeout()), this, SLOT(registerNode()));
  registerNodeTimer->start(1000 * 10);

  checkForInternetConnectivity();
  checkForInternetConnectivityTimer = new QTimer(this);
  connect(checkForInternetConnectivityTimer, SIGNAL(timeout()), this,
          SLOT(checkForInternetConnectivity()));
  checkForInternetConnectivityTimer->start(1000 * 10);

  uploadPrintJobsTimer = new QTimer(this);
  connect(uploadPrintJobsTimer, SIGNAL(timeout()), this,
          SLOT(uploadPrintJobs()));

  updateUserDataTimer = new QTimer(this);
  connect(updateUserDataTimer, SIGNAL(timeout()), this,
          SLOT(getUserDataUpdate()));

  printServer = new PrintSubmissionServer(this);

  if (printServer->start()) {
    LOG_SETTING("IPC Server Name", LIBKI_PRINT_SERVER_NAME);
  } else {
    log::Warning(NIL_ID, "Print submission server could not be started.");
  }

  connect(printServer, SIGNAL(submitPrintRequested(SubmitPrintRequest)), this,
          SLOT(handlePrintRequest(SubmitPrintRequest)));

  connect(printServer,
          SIGNAL(printInfoRequested(PrintInfoRequest, QLocalSocket *)), this,
          SLOT(handlePrintInfoRequest(PrintInfoRequest, QLocalSocket *)));

  LEAVE_FUNC
}

void NetworkClient::attemptLogin(QString aUsername, QString aPassword,
                                 bool createGuest) {
  ENTER_FUNC

  username = aUsername;
  password = aPassword;

  QUrl url = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("version", VERSION);
  query.addQueryItem("action", "login");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  query.addQueryItem("createGuest", createGuest ? "1" : "0");
  url.setQuery(query);

  LOG_SETTING("Login URL", url.toString());
  // qDebug() << "NetworkClient::attemptLogin";

  QNetworkAccessManager *nam;
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processAttemptLoginReply(QNetworkReply *)));
  QObject::connect(
      nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
      SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));

  /*QNetworkReply* reply = */ nam->get(buildRequest(url));
  LEAVE_FUNC
}

void NetworkClient::processAttemptLoginReply(QNetworkReply *reply) {
  ENTER_FUNC

  handleNetworkReplyErrors(reply);

  QByteArray result;
  result = reply->readAll();

  QScriptValue sc;
  QScriptEngine engine;
  sc = engine.evaluate("(" + QString(result) + ")");

  if (sc.property("authenticated").toBoolean() == true) {
    log::info("Login Authenticated", AUTHRESULT_ID,
              SData::new_authresult(true, "-"));

    int units = sc.property("units").toInteger();
    int hold_items_count = sc.property("hold_items_count").toInteger();

    QString aUsername = sc.property("username").toString();
    QString aPassword = sc.property("password").toString();
    if (aUsername.length() && aPassword.length()) {
      username = aUsername;
      password = aPassword;
    }

    doLoginTasks(units, hold_items_count);
  } else {
    QString errorCode = sc.property("error").toString();
    log::info(QString("Login failed. Error Code: %1").arg(errorCode),
              AUTHRESULT_ID, SData::new_authresult(false, errorCode));

    username.clear();
    password.clear();

    emit loginFailed(errorCode);
  }

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();

  LEAVE_FUNC
}

void NetworkClient::attemptLogout() {
  ENTER_FUNC

  QNetworkAccessManager *nam;
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processAttemptLogoutReply(QNetworkReply *)));
  QObject::connect(
      nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
      SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));

  QUrl url = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("version", VERSION);
  query.addQueryItem("action", "logout");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);

  /*QNetworkReply* reply =*/nam->get(buildRequest(url));

  LEAVE_FUNC
}

void NetworkClient::processAttemptLogoutReply(QNetworkReply *reply) {
  ENTER_FUNC

  handleNetworkReplyErrors(reply);

  QByteArray result;
  result = reply->readAll();

  QScriptValue sc;
  QScriptEngine engine;
  sc = engine.evaluate("(" + QString(result) + ")");

  if (sc.property("logged_out").toBoolean() == true) {
    doLogoutTasks();
  } else {
    emit logoutFailed();
  }

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();

  LEAVE_FUNC
}

void NetworkClient::getUserDataUpdate() {
  ENTER_FUNC

  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processGetUserDataUpdateReply(QNetworkReply *)));
  QObject::connect(
      nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
      SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));

  QUrl url = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("version", VERSION);
  query.addQueryItem("action", "get_user_data");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);

  /*QNetworkReply* reply =*/nam->get(buildRequest(url));

  LEAVE_FUNC
}

void NetworkClient::processGetUserDataUpdateReply(QNetworkReply *reply) {
  ENTER_FUNC

  handleNetworkReplyErrors(reply);

  QByteArray result;
  result = reply->readAll();

  qDebug() << "Server Result: " << result;

  QJsonDocument jd = QJsonDocument::fromJson(result);

  if (jd.isObject()) {
    QJsonObject jo = jd.object();

    QString status = jo["status"].toString();
    LOG_SETTING("Server Result:status", status);

    if (status == "Logged in") {
      QJsonArray messages = jo["messages"].toArray();
      LOG_SETTING("Server Result:messages/size", QString(messages.size()));

      for (int i = 0; i < messages.size(); i++) {
        QString m = messages[i].toString();
        LOG_SETTING(QString("Server Result:messages/%1").arg(i), m);
        emit messageRecieved(m);
      }

      QJsonValueRef units_json = jo["units"];
      QVariant units_variant = units_json.toVariant();
      int units = units_variant.toInt();
      LOG_SETTING("Server Result:status/units", QString(units));

      // TODO: This seems like a lot of redundant logging, is it really
      // necessary? qDebug() << "UNITS JASON: " << units_json; qDebug() <<
      // "UNITS VARIANT: " << units_variant; qDebug() << "UNITS: " << units;

      emit timeUpdatedFromServer(units);

      if (units < 1) {
        doLogoutTasks();
      }
    } else if (status == "Logged out") {
      doLogoutTasks();
    } else if (status == "Kicked") {
      doLogoutTasks();
    }
  }

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();

  LEAVE_FUNC
}

void NetworkClient::uploadPrintJobs() {
  ENTER_FUNC

  QSettings printerSettings;
  printerSettings.beginGroup("printers");
  QStringList printers = printerSettings.allKeys();
  LOG_SETTING("printers", printers.join(","));

  foreach (const QString &printer, printers) {
    QString directory = printerSettings.value(printer).toString();

    log::debug(QString("Found a printer: %1").arg(printer), PRINTERFOUND_ID,
               SData::new_printer(printer, directory));

    QDir dir(directory);

    if (!dir.exists()) {
      log::debug(QString("Directory does not exist: %1").arg(directory));
      bool s = dir.mkpath(directory);
      log::debug(QString("Attempt to create directory result: %1").arg(s));
    }

    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      QString absoluteFilePath = fileInfo.absoluteFilePath();

      SubmitPrintRequest request;

      request.filename = absoluteFilePath;
      request.printer = printer;
      request.copies = 0;
      request.pageCount = 0;

      uploadPrintJob(request);
    }
  }
  LEAVE_FUNC
}

void NetworkClient::uploadPrintJob(const SubmitPrintRequest &request) {
  ENTER_FUNC

  QFile *file = new QFile(request.filename);
  bool opened = file->open(QIODevice::ReadOnly);
  if (!opened) {
    log::debug(QString("Opening file %1 failed. Skipping file.")
                   .arg(request.filename));
    return;
  }
  QFileInfo fileInfo = request.filename;
  QString fileNameOnly = fileInfo.fileName();

  // If the file is less than 1 kb, it's still being written. An empty PDF is
  // about 3.7K
  if (fileInfo.size() < 2048) {
    log::debug(
        QString("File size too small: %1. Likely still writing. Skipping File.")
            .arg(fileNameOnly));
    return;
  }

  // If the file is not writable, the print driver hasn't finished writing the
  // PDF
  if (!fileInfo.isWritable()) {
    log::debug(
        QString("File not writable: %1. Likely still writing. Skipping File.")
            .arg(fileNameOnly));
    return;
  }
  const QString printedFileSuffix = ".printed";

  if (request.filename.endsWith(printedFileSuffix)) {
    return;
  }
  qDebug() << "SENDING PRINT JOB: " << fileNameOnly;

  // I only log this here, finally, after all the guards have passed
  log::info("Uploading new print job.", PRINTJOBSEND_ID,
            SData::new_printjob(request));

  QString fileCounterString = QString::number(fileCounter);
  fileCounter++;

  QString newAbsoluteFilePath =
      request.filename + "." + fileCounterString + printedFileSuffix;
  bool renamed = file->rename(request.filename, newAbsoluteFilePath);
  if (!renamed) {
    log::warn(QString("Rename from %1 to %1 failed.")
      .arg(request.filename, newAbsoluteFilePath));
  }

  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  // We con't delete the file object now, delete it with the multiPart
  file->setParent(multiPart);

  QHttpPart clientNamePart;
  clientNamePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant("form-data; name=client_name"));
  QByteArray clientNameQBA;
  clientNameQBA.append(nodeName);
  clientNamePart.setBody(clientNameQBA);
  multiPart->append(clientNamePart);

  QHttpPart userNamePart;
  userNamePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                         QVariant("form-data; name=username"));
  QByteArray userNameQBA;
  userNameQBA.append(username);
  userNamePart.setBody(userNameQBA);
  multiPart->append(userNamePart);

  QHttpPart printerNamePart;
  printerNamePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant("form-data; name=printer"));
  QByteArray printerNameQBA;
  printerNameQBA.append(request.printer);
  printerNamePart.setBody(printerNameQBA);
  multiPart->append(printerNamePart);

  QHttpPart printJobPart;
  printJobPart.setHeader(
      QNetworkRequest::ContentDispositionHeader,
      QVariant("form-data; name=print_file; filename=" + fileNameOnly));
  printJobPart.setBodyDevice(file);
  multiPart->append(printJobPart);

  QHttpPart fileNamePart;
  fileNamePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                         QVariant("form-data; name=filename"));
  QByteArray fileNameQBA;
  fileNameQBA.append(fileNameOnly);
  fileNamePart.setBody(fileNameQBA);
  multiPart->append(fileNamePart);

  QHttpPart copiesPart;
  copiesPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"copies\""));
  copiesPart.setBody(QByteArray::number(request.copies));
  multiPart->append(copiesPart);

  QUrl printUrl = QUrl(serviceURL);
  printUrl.setPath("/api/client/v1_0/print");
  QNetworkRequest netrequest = buildRequest(printUrl);

  QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
  QObject::connect(
      networkManager,
      SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
      SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));

  QNetworkReply *reply = networkManager->post(netrequest, multiPart);
  multiPart->setParent(reply);  // delete the multiPart with the reply

  // TODO: delete file after finished signal emits
  // https://stackoverflow.com/questions/5153157/passing-an-argument-to-a-slot
  connect(networkManager, SIGNAL(finished(QNetworkReply *)), this,
          SLOT(uploadPrintJobReply(QNetworkReply *)));
  connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this,
          SLOT(handleUploadProgress(qint64, qint64)));

  LEAVE_FUNC
}

void NetworkClient::handleUploadProgress(qint64 bytesSent, qint64 bytesTotal) {
  log::debug(QString("Uploaded %1 of %2.")
    .arg(bytesSent)
    .arg(bytesTotal));
}

// TODO: It would be really neat if we could tie the reply to the original print job.
// It would make logging the lifecycle of a print a lot easier.
void NetworkClient::uploadPrintJobReply(QNetworkReply *reply) {
  ENTER_FUNC

  handleNetworkReplyErrors(reply);

  if (reply->error() == QNetworkReply::NoError) {
    log::info("Print successfully uploaded.",
      PRINTJOBRESULT_ID,
      SData::new_serverreply(*reply));

    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();
  } else {
    log::warn(
      QString("Print upload failed: %1. Retrying.").arg(reply->errorString()),
      PRINTJOBRESULT_ID,
      SData::new_serverreply(*reply));

    QNetworkRequest request = reply->request();

    QHttpMultiPart *multiPart = reply->findChild<QHttpMultiPart *>();
    log::debug(QString("Found multiPart"));

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QObject::connect(
        networkManager,
        SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
        SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));

    QNetworkReply *reply = networkManager->post(request, multiPart);

    multiPart->setParent(reply);  // delete the multiPart with the reply

    connect(networkManager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(uploadPrintJobReply(QNetworkReply *)));
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this,
            SLOT(handleUploadProgress(qint64, qint64)));
  };

  LEAVE_FUNC
}

void NetworkClient::registerNode() {
  ENTER_FUNC

  QNetworkAccessManager *nam;
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processRegisterNodeReply(QNetworkReply *)));

  QObject::connect(nam, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)),
                   this,
                   SLOT(handleSslErrors(QNetworkReply *, QList<QSslError>)));

  QUrl url = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("version", VERSION);
  query.addQueryItem("action", "register_node");
  query.addQueryItem("node_name", nodeName);
  query.addQueryItem("age_limit", nodeAgeLimit);
  url.setQuery(query);

  log::info(
    QString("Registering Node to %1").arg(serviceURL.toString()),
    REGISTERNODE_ID,
    SData::new_node(serviceURL.toString(), nodeName, VERSION, nodeAgeLimit));

  /*QNetworkReply* reply =*/nam->get(buildRequest(url));

  LEAVE_FUNC
}

void NetworkClient::handleSslErrors(QNetworkReply *reply,
                                    QList<QSslError> error) {
  reply->ignoreSslErrors(error);
}

void NetworkClient::processRegisterNodeReply(QNetworkReply *reply) {
  ENTER_FUNC

  handleNetworkReplyErrors(reply);

  QByteArray result;
  result = reply->readAll();

  qDebug() << "Server Result: " << result;

  QScriptValue sc;
  QScriptEngine engine;
  sc = engine.evaluate("(" + QString(result) + ")");

  if (!sc.property("registered").toBoolean()) {
    log::warn("Node registration failed.",
      REGISTERNODEREPLY_ID,
      SData::new_serverreply(*reply));
  } else {
    log::info("Node registration successful.",
      REGISTERNODEREPLY_ID,
      SData::new_serverreply(*reply));
  }

  // session unlocking
  if (sc.property("unlock").toBoolean()) {
    log::info("Unlocking due to server request.", NODEUNLOCK_ID);
    emit unlockSession();
  }

  // auto guest creation (formerly 'unlock')
  if (sc.property("autologin").toBoolean()) {
    username = sc.property("username").toString();
    log::info(
      QString("Automatically logging in %1 due to server request.").arg(username),
      NODEUNLOCK_ID);
    doLoginTasks(sc.property("minutes").toInteger(), 0);
  }

  if (sc.property("shutdown").toBoolean()) {
    log::info("Received shutdown message from server.",
      NODEPOWER_ID,
      SData::new_nodepower("shutdown"));
    emit allowClose(true);

#ifdef Q_OS_WIN
    QProcess::startDetached("shutdown -s -f -t 0");
#endif  // ifdef Q_OS_WIN

#ifdef Q_OS_UNIX
    // For this to work, sudo must be installed and the line
    // %shutdown ALL=(root) NOPASSWD: /sbin/reboot FIXME
    // needs to be added to /etc/sudoers
    QProcess::startDetached("sudo shutdown 0");
#endif  // ifdef Q_OS_UNIX
  }

  if (sc.property("suspend").toBoolean()) {
    log::info("Received suspend message from server.",
      NODEPOWER_ID,
      SData::new_nodepower("suspend"));
#ifdef Q_OS_WIN
    QProcess::startDetached("rundll32.exe powrprof.dll,SetSuspendState 0,1,0");
#endif  // ifdef Q_OS_WIN

#ifdef Q_OS_UNIX
    QProcess::startDetached("systemctl suspend -i");
#endif  // ifdef Q_OS_UNIX
  }

  if (sc.property("restart").toBoolean()) {
    log::info("Received restart message from server.",
      NODEPOWER_ID,
      SData::new_nodepower("restart"));
    emit allowClose(true);

#ifdef Q_OS_WIN
    QProcess::startDetached("shutdown -r -f -t 0");
#endif  // ifdef Q_OS_WIN

#ifdef Q_OS_UNIX
    // For this to work, sudo must be installed and the line
    // %shutdown ALL=(root) NOPASSWD: /sbin/reboot
    // needs to be added to /etc/sudoers
    QProcess::startDetached("sudo reboot");
#endif  // ifdef Q_OS_UNIX
  }

  if (sc.property("wakeup").toBoolean()) {
    log::info("Received wakeup message from server.",
      NODEPOWER_ID,
      SData::new_nodepower("wakeup"));
    QStringList MAC_addresses = sc.engine()->fromScriptValue<QStringList>(
        sc.property("wol_mac_addresses"));
    wakeOnLan(MAC_addresses, sc.property("wol_host").toString(),
              sc.property("wol_port").toInteger());
  }

  if (sc.property("drop").toBoolean()) {
    log::info("Received bypass message from server.",
      BYPASS_ID,
      SData::new_bypassattempt("-", "-", true));
#ifdef Q_OS_WIN
    QProcess::startDetached("c:/windows/explorer.exe");
    QProcess::startDetached("windows/on_login.exe");
#endif  // ifdef Q_OS_WIN
    exit(1);
  }

  QString styleSheet = sc.property("ClientStyleSheet").toString();
  if (!styleSheet.isEmpty()) {
    this->app->setStyleSheet(styleSheet);
  }

  QSettings settings;
  settings.setIniCodec("UTF-8");

  QString bannerTopURL = settings.value("session/BannerTopURL").toString();
  QString bannerBottomURL =
      settings.value("session/BannerBottomURL").toString();

  settings.setValue("session/ClientBehavior",
                    sc.property("ClientBehavior").toString());
  settings.setValue("session/ReservationShowUsername",
                    sc.property("ReservationShowUsername").toString());
  settings.setValue("session/EnableClientSessionLocking",
                    sc.property("EnableClientSessionLocking").toString());
  settings.setValue("session/EnableClientPasswordlessMode",
                    sc.property("EnableClientPasswordlessMode").toString());
  settings.setValue("session/TermsOfService",
                    sc.property("TermsOfService").toString());
  settings.setValue("session/TermsOfServiceDetails",
                    sc.property("TermsOfServiceDetails").toString());

  settings.setValue("session/BannerTopURL",
                    sc.property("BannerTopURL").toString());
  settings.setValue("session/BannerTopWidth",
                    sc.property("BannerTopWidth").toString());
  settings.setValue("session/BannerTopHeight",
                    sc.property("BannerTopHeight").toString());

  settings.setValue("session/BannerBottomURL",
                    sc.property("BannerBottomURL").toString());
  settings.setValue("session/BannerBottomWidth",
                    sc.property("BannerBottomWidth").toString());
  settings.setValue("session/BannerBottomHeight",
                    sc.property("BannerBottomHeight").toString());

  settings.setValue("session/LogoURL", sc.property("Logo").toString());
  settings.setValue("session/LogoWidth", sc.property("LogoWidth").toString());
  settings.setValue("session/LogoHeight", sc.property("LogoHeight").toString());

  settings.setValue("session/inactivityLogout",
                    sc.property("inactivityLogout").toString());
  settings.setValue("session/inactivityWarning",
                    sc.property("inactivityWarning").toString());

  settings.setValue("session/InternetConnectivityURLs",
                    sc.property("InternetConnectivityURLs").toString());

  settings.setValue("session/EnableGuestSelfRegistration",
                    sc.property("EnableGuestSelfRegistration").toString());

  settings.setValue("session/ShowTimeRemainingInSplash",
                    sc.property("ShowTimeRemainingInSplash").toString());
  settings.setValue("session/ShowTimeRemainingInTray",
                    sc.property("ShowTimeRemainingInTray").toString());

  settings.setValue("session/ClientTimeNotificationFrequency",
                    sc.property("ClientTimeNotificationFrequency").toString());
  settings.setValue("session/ClientTimeWarningThreshold",
                    sc.property("ClientTimeWarningThreshold").toString());
  settings.setValue("session/ClientTimeWarningFrequency",
                    sc.property("ClientTimeWarningFrequency").toString());

  QString logoURL = settings.value("images/logo").toString();

  QString guestRegistrationEnabled =
      settings.value("session/EnableGuestSelfRegistration").toString();

  settings.sync();

  if ((logoURL != sc.property("Logo").toString()) ||
      (bannerTopURL != sc.property("BannerTopURL").toString()) ||
      (bannerBottomURL != sc.property("BannerBottomURL").toString()) ||
      (guestRegistrationEnabled !=
       sc.property("EnableGuestSelfRegistration").toString())) {
    emit handleBanners();  // TODO: Emit only if a banner url has changed
  }

  QString reserved_for = sc.property("reserved_for").toString();
  emit setReservationStatus(reserved_for);

  QString status = sc.property("status").toString();
  if (status != clientStatus) {
    if (status == "suspended") {
      emit clientSuspended();
    } else if (status == "online") {
      emit clientOnline();
    }
  }
  clientStatus = status;

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();

  LEAVE_FUNC
}

void NetworkClient::checkForInternetConnectivity() {
  ENTER_FUNC

  QList<QString> list;

  QSettings settings;
  settings.setIniCodec("UTF-8");
  QString internetConnectivityURLs =
      settings.value("session/InternetConnectivityURLs").toString();
  // qDebug() << "URLS: " << internetConnectivityURLs;
  if (internetConnectivityURLs != "null") {
    list = internetConnectivityURLs.split(QRegExp("[\r\n]"),
                                          QString::SkipEmptyParts);
  }
  // qDebug() << "URLS LIST: " << list.join(" ");

  if (list.size()) {
    // Select a URL from the list at random to test connectivity
    QString url = list.at(qrand() % list.size());

    log::debug(
      QString("Checking URL: %1").arg(url),
      CHECKINTERNET_ID,
      SData::new_checkinternet(url));

    QNetworkAccessManager *nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(
        nam, SIGNAL(finished(QNetworkReply *)), this,
        SLOT(processCheckForInternetConnectivityReply(QNetworkReply *)));
    QObject::connect(
        nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
        SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));

    nam->get(QNetworkRequest(QUrl(url)));
  }

  LEAVE_FUNC
}

void NetworkClient::processCheckForInternetConnectivityReply(
    QNetworkReply *reply) {
  ENTER_FUNC

  if (reply->error() != QNetworkReply::NoError) {
    log::warn(
      QString("Internet check failed: %1").arg(reply->errorString()),
      CHECKINTERNETREPLY_ID,
      SData::new_serverreply(*reply));
    emit internetAccessWarning(reply->errorString());
  } else {
    log::debug("Internet check succeeded.",
      CHECKINTERNETREPLY_ID,
      SData::new_serverreply(*reply));
    emit internetAccessWarning("");
  }

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();

  LEAVE_FUNC
}

void NetworkClient::clearMessage() {
  ENTER_FUNC

  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(ignoreNetworkReply(QNetworkReply *)));
  QObject::connect(
      nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
      SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));
  QUrl url = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("version", VERSION);
  query.addQueryItem("action", "clear_message");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);
  nam->get(buildRequest(url));

  LEAVE_FUNC
}

void NetworkClient::acknowledgeReservation(QString reserved_for) {
  ENTER_FUNC

  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(ignoreNetworkReply(QNetworkReply *)));
  QObject::connect(
      nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this,
      SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));
  QUrl url = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("version", VERSION);
  query.addQueryItem("action", "acknowledge_reservation");
  query.addQueryItem("reserved_for", reserved_for);
  url.setQuery(query);

  log::info(
    QString("Acknowledging reservation for %1.").arg(reserved_for),
    RESERVATIONACK_ID,
    SData::new_reservation(reserved_for));

  nam->get(buildRequest(url));

  LEAVE_FUNC
}

void NetworkClient::ignoreNetworkReply(QNetworkReply *reply) {
  ENTER_FUNC

  handleNetworkReplyErrors(reply);

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();

  LEAVE_FUNC
}

void NetworkClient::doLoginTasks(int units, int hold_items_count) {
  ENTER_FUNC

#ifdef Q_OS_WIN
  // FIXME: We should delete print jobs at login as well in case a client crash
  // prevented the print jobs for getting cleaned up at logout time

  // If this is an MS Windows platform, use the keylocker programs to limit
  // mischief.
  QProcess::startDetached("c:/windows/explorer.exe");
  QProcess::startDetached("windows/on_login.exe");
#endif  // ifdef Q_OS_WIN

  uploadPrintJobsTimer->start(1000 * 2);
  updateUserDataTimer->start(1000 * 10);

  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.setValue("session/LoggedInUser", username);
  settings.sync();

  QString scriptlogin_enable = settings.value("scriptlogin/enable").toString();
  LOG_SETTING("scriptlogin/enable", scriptlogin_enable);
  if (scriptlogin_enable == "1") {
    QString loginscript = settings.value("scriptlogin/script").toString();
    LOG_SETTING("scriptlogin/script", loginscript);
    QProcess::startDetached(loginscript);
  }
  emit loginSucceeded(username, password, units, hold_items_count);

  LEAVE_FUNC
}

void NetworkClient::doLogoutTasks() {
  ENTER_FUNC

  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.setValue("session/LoggedInUser", "");
  settings.sync();

  // Delete print jobs
  QSettings printerSettings;
  printerSettings.beginGroup("printers");
  QStringList printers = printerSettings.allKeys();
  foreach (const QString &printer, printers) {
    QString directory = printerSettings.value(printer).toString();
    QDir dir(directory);

    dir.setFilter(QDir::Files);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      QString absoluteFilePath = fileInfo.absoluteFilePath();
      QFile::remove(absoluteFilePath);
    }
  }

  uploadPrintJobsTimer->stop();
  updateUserDataTimer->stop();

  username.clear();
  password.clear();

#ifdef Q_OS_WIN

  // If this is an MS Windows platform, use the keylocker programs to limit
  // mischief.
  QProcess::startDetached("taskkill /f /im explorer.exe");
  QProcess::startDetached("windows/on_logout.exe");

  if (actionOnLogout == LogoutAction::Logout) {
    log::info("Logging out due to actionOnLogout.",
      NODEPOWER_ID,
      SData::new_nodepower("logout"));
    emit allowClose(true);
    QProcess::startDetached("shutdown -l -f");
  } else if (actionOnLogout == LogoutAction::Reboot) {
    log::info("Restarting due to actionOnLogout.",
      NODEPOWER_ID,
      SData::new_nodepower("restart"));
    emit allowClose(true);
    QProcess::startDetached("shutdown -r -f -t 0");
  }
#endif  // ifdef Q_OS_WIN

#ifdef Q_OS_UNIX

  if (actionOnLogout == LogoutAction::Logout) {
    emit allowClose(true);

    // Restart KDE 4
    QProcess::startDetached(
        "qdbus org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout "
        "-0 -1 -1");

    // Restart Gnome
    QProcess::startDetached("gnome-session-save --kill --silent");

    // Restart Unity
    QProcess::startDetached("gnome-session-quit --no-prompt");

    // Restart XFCE 4
    QProcess::startDetached("/usr/bin/xfce4-session-logout");

    // Restart Mate
    QProcess::startDetached("mate-session-save --force-logout");
  } else if (actionOnLogout == LogoutAction::Reboot) {
    emit allowClose(true);

    // For this to work, sudo must be installed and the line
    // %shutdown ALL=(root) NOPASSWD: /sbin/reboot
    // needs to be added to /etc/sudoers
    QProcess::startDetached("sudo reboot");
  }
#endif  // ifdef Q_OS_UNIX
  QString logout_script = settings.value("scriptlogout/enable").toString();
  LOG_SETTING("scriptlogout/enable", logout_script);
  if (settings.value("scriptlogout/enable").toString() == "1") {
    QProcess::startDetached(settings.value("scriptlogout/script").toString());
  }
  emit logoutSucceeded();

  LEAVE_FUNC
}

void NetworkClient::wakeOnLan(QStringList MAC_addresses, QString host,
                              qint64 port) {
  ENTER_FUNC

  QHostAddress host_address;
  host_address.setAddress(host);

  for (int i = 0; i < MAC_addresses.size(); i++) {
    char address[6];
    char packet[102];

    memset(packet, 0xff, 6);

    for (int j = 0; j < 6; j++) {
      address[j] = MAC_addresses.at(i).section(":", j, j).toInt(Q_NULLPTR, 16);
    }

    for (int j = 1; j <= 16; j++) {
      memcpy(&packet[j * 6], &address, 6 * sizeof(char));
    }

    QUdpSocket udpSocket;
    udpSocket.writeDatagram(packet, 102, host_address, port);
  }

  LEAVE_FUNC
}

QNetworkRequest NetworkClient::buildRequest(const QUrl &url) const {
  QNetworkRequest request(url);
  if (!customHeaderName.isEmpty()) {
    request.setRawHeader(customHeaderName.toUtf8(), customHeaderValue.toUtf8());
  }
  return request;
}

void NetworkClient::handleNetworkReplyErrors(QNetworkReply *reply) {
  if (reply->error() != QNetworkReply::NoError) {
    QString e = QString::number(reply->error());
    qWarning() << "ERROR: Server Access Warning: " << e
               << " :: " << reply->errorString();

    QString s = e + ": " + reply->errorString();
    serverAccessWarning(s);
  } else {
    serverAccessWarning("");
  }
}

void NetworkClient::handlePrintRequest(const SubmitPrintRequest &request) {
  if (username.isEmpty()) {
    qWarning() << "Ignoring print request because no user is logged in.";
    return;
  }

  uploadPrintJob(request);
}

void NetworkClient::handlePrintInfoRequest(PrintInfoRequest request,
                                           QLocalSocket *socket) {
  ENTER_FUNC

  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QUrl url(serviceURL);

  url.setPath("/api/client/v1_0/print_price_check");

  QUrlQuery query;

  query.addQueryItem("client_name", nodeName);
  query.addQueryItem("username", username);
  query.addQueryItem("printer", request.printer);

  url.setQuery(query);

  QNetworkReply *reply = nam->get(buildRequest(url));

  PendingPrintInfoRequest context;

  context.socket = socket;
  context.request = request;

  pendingPrintInfoReplies.insert(reply, context);

  connect(reply, SIGNAL(finished()), this, SLOT(processPrintPriceCheckReply()));

  LEAVE_FUNC
}

void NetworkClient::processPrintPriceCheckReply() {
  ENTER_FUNC

  QNetworkReply *networkReply = qobject_cast<QNetworkReply *>(sender());

  if (!networkReply) {
    qWarning("No network reply");
    return;
  }
  PendingPrintInfoRequest context = pendingPrintInfoReplies.take(networkReply);

  PrintInfoReply reply;

  if (networkReply->error() != QNetworkReply::NoError) {
    reply.success = false;
    reply.error = networkReply->errorString();
    qWarning() << "Network reply error: " << reply.error;

    printServer->sendPrintInfoReply(context.socket, reply);

    networkReply->deleteLater();
    return;
  }

  QJsonParseError parseError;

  QJsonDocument doc =
      QJsonDocument::fromJson(networkReply->readAll(), &parseError);

  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    reply.success = false;
    reply.error = tr("Invalid JSON returned by server.");
    qWarning() << "JSON parsing error";

    printServer->sendPrintInfoReply(context.socket, reply);

    networkReply->deleteLater();
    return;
  }

  QJsonObject obj = doc.object();

  reply.success = true;
  reply.printer = obj.value("printer").toString();
  reply.currency = obj.value("currency").toString();
  reply.costPerPage = obj.value("cpp").toDouble();
  reply.availableFunds = obj.value("funds").toDouble();
  reply.availableGratis = obj.value("gratis_balance").toDouble();
  reply.gratisMethod = obj.value("gratis_method").toString();

  int totalPages = context.request.pageCount * context.request.copies;
  reply.estimatedCost = reply.costPerPage * totalPages;

  if (reply.gratisMethod == "pages") {
    if (totalPages <= reply.availableGratis) {
      reply.estimatedCost = 0;
      reply.remainingGratisBalance = reply.availableGratis - totalPages;
    } else {
      reply.estimatedCost =
          reply.costPerPage * (totalPages - reply.availableGratis);
      reply.remainingGratisBalance = 0;
    }
  } else if (reply.gratisMethod == "funds") {
    if (reply.estimatedCost <= reply.availableGratis) {
      reply.remainingGratisBalance =
          reply.availableGratis - reply.estimatedCost;
      reply.estimatedCost = 0;
    } else {
      reply.estimatedCost =
          (reply.costPerPage * totalPages) - reply.availableGratis;
      reply.remainingGratisBalance = 0;
    }
  } else {
    qWarning() << "Invalid gratis method: " << reply.gratisMethod;
  }

  reply.remainingFundsBalance = reply.availableFunds - reply.estimatedCost;

  reply.canPrint = (reply.remainingFundsBalance >= 0.0);

  printServer->sendPrintInfoReply(context.socket, reply);

  networkReply->deleteLater();

  LEAVE_FUNC
}
