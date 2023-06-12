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

#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QHash>
#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJSEngine>

namespace LogoutAction {
enum Enum { Logout, Reboot, NoAction };
}

class NetworkClient : public QObject {
  Q_OBJECT

 public:
  NetworkClient(QApplication *app);

 signals:

  void loginSucceeded(const QString &username, const QString &password,
                      const int &minutes, const int &hold_items_count);
  void loginFailed(QString errorCode);
  void timeUpdatedFromServer(int minutes);
  void logoutSucceeded();
  void logoutFailed();
  void messageRecieved(QString message);
  void allowClose(bool);
  void setReservationStatus(QString reserved_for);
  void handleBanners();
  void clientSuspended();
  void clientOnline();
  void serverAccessWarning(QString);
  void internetAccessWarning(QString);

 public slots:

  void attemptLogin(QString username, QString password);
  void attemptLogout();
  void acknowledgeReservation(QString reserved_for);

 private slots:

  void registerNode();
  void processRegisterNodeReply(QNetworkReply *reply);

  void uploadPrintJobs();

  void getUserDataUpdate();
  void processGetUserDataUpdateReply(QNetworkReply *reply);

  void clearMessage();

  void ignoreNetworkReply(QNetworkReply *reply);
  void uploadPrintJobReply(QNetworkReply *reply);
  void handleUploadProgress(qint64, qint64);

  void processAttemptLoginReply(QNetworkReply *reply);
  void processAttemptLogoutReply(QNetworkReply *reply);

  void handleSslErrors(QNetworkReply *reply, QList<QSslError> error);

  void checkForInternetConnectivity();
  void processCheckForInternetConnectivityReply(QNetworkReply *reply);

  void handleNetworkReplyErrors(QNetworkReply *reply);

 private:
  QApplication *app;

  QTimer *registerNodeTimer;
  QTimer *uploadPrintJobsTimer;
  QTimer *updateUserDataTimer;
  QTimer *checkForInternetConnectivityTimer;

  QUrl serviceURL;
  QUrlQuery urlQuery;

  QString nodeName;
  QString nodeLocation;
  QString nodeType;
  QString nodeAgeLimit;
  QString nodeIPAddress;
  QString nodeMACAddress;
  QString nodeHostname;

  LogoutAction::Enum actionOnLogout;

  QString clientStatus;

  QString username;
  QString password;

  int fileCounter;

  void doLoginTasks(int units, int hold_items_count);
  void doLogoutTasks();

  void wakeOnLan(QStringList MAC_addresses, QString host, qint64 port);

};

#endif  // NETWORKCLIENT_H
