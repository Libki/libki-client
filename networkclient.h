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
#include <QObject>
#include <QHash>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QDebug>
#include <QProcess>
#include <QSettings>

namespace LogoutAction {
enum Enum {
  Logout,
  Reboot,
  NoAction
};
}

class NetworkClient : public QObject {
  Q_OBJECT

public:

  NetworkClient();

signals:

  void loginSucceeded(const QString& username,
                      const QString& password,
                      const int    & minutes,
                      const int    & hold_items_count);
  void loginFailed(QString errorCode);
  void timeUpdatedFromServer(int minutes);
  void logoutSucceeded();
  void logoutFailed();
  void messageRecieved(QString message);
  void allowClose(bool);
  void setReservationStatus(QString reserved_for);
  void handleBanners();

public slots:

  void attemptLogin(QString username,
                    QString password);
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

private:

  QTimer *registerNodeTimer;
  QTimer *uploadPrintJobsTimer;
  QTimer *updateUserDataTimer;

  QUrl serviceURL;
  QUrlQuery urlQuery;

  QString nodeName;
  QString nodeLocation;
  QString nodeType;
  QString nodeAgeLimit;

  LogoutAction::Enum actionOnLogout;

  QString username;
  QString password;

  int fileCounter;

  void doLoginTasks(int units,
                    int hold_items_count);
  void doLogoutTasks();
};

#endif // NETWORKCLIENT_H
