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

#include <QtNetwork/QHostInfo>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QHttpMultiPart>

NetworkClient::NetworkClient() : QObject() {
  qDebug("NetworkClient::NetworkClient");

  fileCounter = 0;

  QString os_username;
#ifdef Q_OS_WIN
  os_username = getenv("USERNAME");
#endif // ifdef Q_OS_WIN
#ifdef Q_OS_UNIX
  os_username = getenv("USER");
#endif // ifdef Q_OS_UNIX

  QSettings settings;
  settings.setIniCodec("UTF-8");

  nodeName = settings.value("node/name").toString();

  qDebug() << "OS USERNAME: " << os_username;
  qDebug() << "CONFIG NODE NAME: " << nodeName;

  if (nodeName == "OS_USERNAME") {
    nodeName = os_username;
  }

  // Fail over to hostname if node name isn't defined.
  if (nodeName.isEmpty()) {
    QHostInfo hostInfo;
    hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    nodeName = QHostInfo::localHostName();
  }
  qDebug() << "NODE NAME: " << nodeName;

  nodeLocation = settings.value("node/location").toString();
  qDebug() << "LOCATION: " << nodeLocation;
  nodeType = settings.value("node/type").toString();
  qDebug() << "TYPE: " << nodeType;
  nodeAgeLimit = settings.value("node/age_limit").toString();
  qDebug() << "AGE LIMIT: " << nodeAgeLimit;

  QString action = settings.value("node/logoutAction").toString();

  if (action == "logout") {
    actionOnLogout = LogoutAction::Logout;
  } else if (action == "reboot") {
    actionOnLogout = LogoutAction::Reboot;
  } else {
    actionOnLogout = LogoutAction::NoAction;
  }

  qDebug() << "HOST: " << settings.value("server/host").toString();
  serviceURL.setHost(settings.value("server/host").toString());
  serviceURL.setPort(settings.value("server/port").toInt());
  serviceURL.setScheme(settings.value("server/scheme").toString());
  serviceURL.setPath("/api/client/v1_0");

  urlQuery.addQueryItem("node",     nodeName);
  urlQuery.addQueryItem("location", nodeLocation);
  urlQuery.addQueryItem("type", nodeType);

  registerNode();
  registerNodeTimer = new QTimer(this);
  connect(registerNodeTimer, SIGNAL(timeout()), this, SLOT(registerNode()));
  registerNodeTimer->start(1000 * 10);

  uploadPrintJobsTimer = new QTimer(this);
  connect(uploadPrintJobsTimer, SIGNAL(timeout()), this, SLOT(uploadPrintJobs()));

  updateUserDataTimer = new QTimer(this);
  connect(updateUserDataTimer,  SIGNAL(timeout()), this,
          SLOT(getUserDataUpdate()));
}

void NetworkClient::attemptLogin(QString aUsername, QString aPassword) {
  qDebug("NetworkClient::attemptLogin");

  username = aUsername;
  password = aPassword;

  QUrl url        = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("action",   "login");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);

  qDebug() << "LOGIN URL: " << url.toString();
  qDebug() << "NetworkClient::attemptLogin";

  QNetworkAccessManager *nam;
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processAttemptLoginReply(QNetworkReply *)));

  /*QNetworkReply* reply = */ nam->get(QNetworkRequest(url));
}

void NetworkClient::processAttemptLoginReply(QNetworkReply *reply) {
  qDebug("NetworkClient::processAttemptLogoutReply");

  QByteArray result;
  result = reply->readAll();

  QScriptValue  sc;
  QScriptEngine engine;
  sc = engine.evaluate("(" + QString(result) + ")");

  if (sc.property("authenticated").toBoolean() == true) {
    qDebug("Login Authenticated");

    int units            = sc.property("units").toInteger();
    int hold_items_count = sc.property("hold_items_count").toInteger();

    doLoginTasks(units, hold_items_count);
  } else {
    qDebug("Login Failed");

    QString errorCode = sc.property("error").toString();
    qDebug() << "Error Code: " << errorCode;

    username.clear();
    password.clear();

    emit loginFailed(errorCode);
  }

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();
}

void NetworkClient::attemptLogout() {
  qDebug("NetworkClient::attemptLogout");

  QNetworkAccessManager *nam;
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processAttemptLogoutReply(QNetworkReply *)));

  QUrl url        = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("action",   "logout");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);

  /*QNetworkReply* reply =*/ nam->get(QNetworkRequest(url));
}

void NetworkClient::processAttemptLogoutReply(QNetworkReply *reply) {
  qDebug("NetworkClient::processAttemptLogoutReply");

  QByteArray result;
  result = reply->readAll();

  QScriptValue  sc;
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
}

void NetworkClient::getUserDataUpdate() {
  qDebug("NetworkClient::getUserDataUpdate");

  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processGetUserDataUpdateReply(QNetworkReply *)));

  QUrl url        = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("action",   "get_user_data");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);

  /*QNetworkReply* reply =*/ nam->get(QNetworkRequest(url));
}

void NetworkClient::processGetUserDataUpdateReply(QNetworkReply *reply) {
  qDebug("NetworkClient::processGetUserDataUpdateReply");

  QByteArray result;
  result = reply->readAll();

  qDebug() << "Server Result: " << result;

  QJsonDocument jd = QJsonDocument::fromJson(result);

  if (jd.isObject()) {
    QJsonObject jo = jd.object();

    QString status = jo["status"].toString();
    qDebug() << "STATUS: " << status;

    if (status == "Logged in") {
      QJsonArray messages = jo["messages"].toArray();
      qDebug() << "MESSAGE ARRAY SIZE: " << messages.size();

      for (int i = 0; i < messages.size(); i++) {
        QString m = messages[i].toString();
        qDebug() << "MESSAGE: " << m;
        emit messageRecieved(m);
      }

      QJsonValueRef units_json = jo["units"];
      QVariant units_variant = units_json.toVariant();
      int units = units_variant.toInt();
      qDebug() << "UNITS JASON: " << units_json;
      qDebug() << "UNITS VARIANT: " << units_variant;
      qDebug() << "UNITS: " << units;

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
}

void NetworkClient::uploadPrintJobs() {
  qDebug() << "NetworkClient::uploadPrintJobs";

  QSettings printerSettings;
  printerSettings.beginGroup("printers");
  QStringList printers = printerSettings.allKeys();
  qDebug() << "PRINTER: " << printers;

  foreach(const QString &printer, printers) {
    qDebug() << "FOUND PRINTER: " << printer;
    qDebug() << "PATH: " << printerSettings.value(printer).toString();

    QString directory = printerSettings.value(printer).toString();
    QDir    dir(directory);

    if (!dir.exists()) {
      qDebug() << "Directory does not exist: " << directory;
    }

    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
      QString printedFileSuffix = ".printed";

      QFileInfo fileInfo         = list.at(i);
      QString   absoluteFilePath = fileInfo.absoluteFilePath();
      QString   fileName         = fileInfo.fileName();
      //qDebug() << "Found Print Job File: " << absoluteFilePath;

      if (fileName.endsWith(printedFileSuffix)) {
        //qDebug() << "PRINT JOB ALREADY PROCCESSED: " << fileName;
        continue;
      }
      qDebug() << "SENDING PRINT JOB: " << fileName;

      QString fileCounterString = QString::number(fileCounter);
      fileCounter++;

      QString newAbsoluteFilePath = absoluteFilePath + "." + fileCounterString + printedFileSuffix;
      QFile::rename(absoluteFilePath, newAbsoluteFilePath);

      QFile *file = new QFile(newAbsoluteFilePath);
      file->open(QIODevice::ReadOnly);

      QHttpMultiPart *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);

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
      printerNameQBA.append(printer);
      printerNamePart.setBody(printerNameQBA);
      multiPart->append(printerNamePart);

      QHttpPart printJobPart;
      printJobPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                             QVariant("form-data; name=print_file; filename=" + fileName ));
      printJobPart.setBodyDevice(file);
      multiPart->append(printJobPart);

      QHttpPart fileNamePart;
      fileNamePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                             QVariant("form-data; name=filename"));
      QByteArray fileNameQBA;
      fileNameQBA.append(fileName);
      fileNamePart.setBody(fileNameQBA);
      multiPart->append(fileNamePart);

      QUrl printUrl = QUrl(serviceURL);
      printUrl.setPath("/api/client/v1_0/print");
      QNetworkRequest request(printUrl);

      QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

      QNetworkReply *reply = networkManager->post(request, multiPart);
      multiPart->setParent(reply); // delete the multiPart with the reply

      // TODO: delete file after finished signal emits
      // https://stackoverflow.com/questions/5153157/passing-an-argument-to-a-slot
      connect(networkManager, SIGNAL(finished(QNetworkReply *)), this,
              SLOT(uploadPrintJobReply(QNetworkReply *)));
      connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this,
              SLOT(handleUploadProgress(qint64, qint64)));
    }
  }
}

void NetworkClient::handleUploadProgress(qint64 bytesSent, qint64 bytesTotal) {
    qDebug() << "Uploaded " << bytesSent << "of" << bytesTotal;
}

void NetworkClient::uploadPrintJobReply(QNetworkReply *reply) {
  qDebug("NetworkClient::uploadPrintJobReply");

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();
}

void NetworkClient::registerNode() {
  qDebug("NetworkClient::registerNode");

  QNetworkAccessManager *nam;
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(processRegisterNodeReply(QNetworkReply *)));

  QUrl url        = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("action",    "register_node");
  query.addQueryItem("node_name", nodeName);
  query.addQueryItem("age_limit", nodeAgeLimit);
  url.setQuery(query);

  /*QNetworkReply* reply =*/ nam->get(QNetworkRequest(url));
}

void NetworkClient::processRegisterNodeReply(QNetworkReply *reply) {
  qDebug("NetworkClient::processRegisterNodeReply");

  QByteArray result;
  result = reply->readAll();

  qDebug() << "Server Result: " << result;

  QScriptValue  sc;
  QScriptEngine engine;
  sc = engine.evaluate("(" + QString(result) + ")");

  if (!sc.property("registered").toBoolean()) {
    qDebug("Node Registration FAILED");
  }

  //TODO: Rename this to something like 'auto-login guest session'
  // This feature is not related to session locking
  if (sc.property("unlock").toBoolean()) {
    qDebug("Unlocking...");
    username = sc.property("username").toString();
    doLoginTasks(sc.property("minutes").toInteger(), 0);
  }

  QSettings settings;
  settings.setIniCodec("UTF-8");

  QString bannerTopURL    = settings.value("session/BannerTopURL").toString();
  QString bannerBottomURL = settings.value("session/BannerBottomURL").toString();

  settings.setValue("session/ClientBehavior",
                    sc.property("ClientBehavior").toString());
  settings.setValue("session/ReservationShowUsername",
                    sc.property("ReservationShowUsername").toString());
  settings.setValue("session/EnableClientSessionLocking",
                    sc.property("EnableClientSessionLocking").toString());
  settings.setValue("session/TermsOfService",
                    sc.property("TermsOfService").toString());

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

  settings.setValue("session/inactivityLogout",
                    sc.property("inactivityLogout").toString());
  settings.setValue("session/inactivityWarning",
                    sc.property("inactivityWarning").toString());

  settings.sync();

  if ((bannerTopURL != sc.property("BannerTopURL").toString()) ||
      (bannerBottomURL != sc.property("BannerBottomURL").toString())) {
    emit handleBanners(); // TODO: Emit only if a banner url has changed
  }

  QString reserved_for = sc.property("reserved_for").toString();
  emit    setReservationStatus(reserved_for);

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();
}

void NetworkClient::clearMessage() {
  qDebug("NetworkClient::clearMessage");
  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(ignoreNetworkReply(QNetworkReply *)));
  QUrl url        = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("action",   "clear_message");
  query.addQueryItem("username", username);
  query.addQueryItem("password", password);
  url.setQuery(query);
  nam->get(QNetworkRequest(url));
}

void NetworkClient::acknowledgeReservation(QString reserved_for) {
  qDebug("NetworkClient::acknowledgeReservation");
  QNetworkAccessManager *nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply *)), this,
                   SLOT(ignoreNetworkReply(QNetworkReply *)));
  QUrl url        = QUrl(serviceURL);
  QUrlQuery query = QUrlQuery(urlQuery);
  query.addQueryItem("action",       "acknowledge_reservation");
  query.addQueryItem("reserved_for", reserved_for);
  url.setQuery(query);

  nam->get(QNetworkRequest(url));
}

void NetworkClient::ignoreNetworkReply(QNetworkReply *reply) {
  qDebug("NetworkClient::ignoreNetworkReply");

  reply->abort();
  reply->deleteLater();
  reply->manager()->deleteLater();
}

void NetworkClient::doLoginTasks(int units, int hold_items_count) {
#ifdef Q_OS_WIN
  //FIXME: We should delete print jobs at login as well in case a client crash prevented the print jobs for getting cleaned up at logout time

  // If this is an MS Windows platform, use the keylocker programs to limit
  // mischief.
  QProcess::startDetached("c:/windows/explorer.exe");
  QProcess::startDetached("windows/on_login.exe");
#endif // ifdef Q_OS_WIN

  uploadPrintJobsTimer->start(1000 * 2);
  updateUserDataTimer->start(1000 * 10);

  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.setValue("session/LoggedInUser", username);
  settings.sync();
  qDebug() << "SCRIPTLOGIN:" << settings.value("scriptlogin/enable").toString();
  if (settings.value("scriptlogin/enable").toString() == "1"){
      QProcess::startDetached(settings.value("scriptlogin/script").toString());
  }
  emit loginSucceeded(username, password, units, hold_items_count);
}

void NetworkClient::doLogoutTasks() {
  qDebug("NetworkClient::doLogoutTasks");

  QSettings settings;
  settings.setIniCodec("UTF-8");
  settings.setValue("session/LoggedInUser", "");
  settings.sync();

  // Delete print jobs
  QSettings printerSettings;
  printerSettings.beginGroup("printers");
  QStringList printers = printerSettings.allKeys();
  foreach(const QString &printer, printers) {
    QString directory = printerSettings.value(printer).toString();
    QDir    dir(directory);

    dir.setFilter(QDir::Files);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo         = list.at(i);
      QString   absoluteFilePath = fileInfo.absoluteFilePath();
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
    emit allowClose(true);
    QProcess::startDetached("shutdown -l -f");
  } else if (actionOnLogout == LogoutAction::Reboot) {
    emit allowClose(true);
    QProcess::startDetached("shutdown -r -f -t 0");
  }
#endif // ifdef Q_OS_WIN

#ifdef Q_OS_UNIX

  if (actionOnLogout == LogoutAction::Logout) {
    emit allowClose(true);

    // Restart KDE 4
    QProcess::startDetached(
      "qdbus org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout -0 -1 -1");

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
#endif // ifdef Q_OS_UNIX
  qDebug() << "SCRIPTLOGOUT:" << settings.value("scriptlogout/enable").toString();
  if (settings.value("scriptlogout/enable").toString() == "1"){
      QProcess::startDetached(settings.value("scriptlogout/script").toString());
  }
  emit logoutSucceeded();
}
