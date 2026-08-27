/*
 * Copyright 2020 Maryse Simard <maryse.simard@inlibro.com>
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

#include "utils.h"

#include <QDebug>
#include <QLocale>
#include <QNetworkInterface>
#include <QSettings>
#include <QtNetwork/QHostInfo>

#include "log.h"

QString getLabel(QString labelcode) {
  ENTER_FUNC

  QSettings settings;
  settings.setIniCodec("UTF-8");

  QString locale = QLocale::system().name();
  QString label = QString();

  if (!settings.value("labels-" + locale + "/" + labelcode)
           .toString()
           .isEmpty()) {
    label = settings.value("labels-" + locale + "/" + labelcode).toString();
  } else if (!settings
                  .value("labels-" + locale.left(locale.indexOf('_')) + "/" +
                         labelcode)
                  .toString()
                  .isEmpty()) {
    label = settings
                .value("labels-" + locale.left(locale.indexOf('_')) + "/" +
                       labelcode)
                .toString();
  } else if (!settings.value("labels/" + labelcode).toString().isEmpty()) {
    label = settings.value("labels/" + labelcode).toString();
  }

  LEAVE_FUNC
  return label;
}

QString clientName = "";
QString getClientName() {
  ENTER_FUNC

  if (clientName.length() == 0) {
    QSettings settings;
    settings.setIniCodec("UTF-8");

    QString os_username;
#ifdef Q_OS_WIN
    os_username = getenv("USERNAME");
#endif  // ifdef Q_OS_WIN
#ifdef Q_OS_UNIX
    os_username = getenv("USER");
#endif  // ifdef Q_OS_UNIX

    clientName = settings.value("node/name").toString();
    LOG_SETTING("Username", os_username);
    LOG_SETTING("node/name", clientName);

    if (clientName == "OS_USERNAME") {
      clientName = os_username;
    }

    // Fail over to hostname if node name isn't defined.
    if (clientName.isEmpty()) {
      QHostInfo hostInfo;
      hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
      clientName = QHostInfo::localHostName();
    }
    LOG_SETTING("Node name", clientName);
  }

  LEAVE_FUNC
  return clientName;
}

QNetworkInterface getNetworkInterface() {
  QNetworkInterface netInterface;

  foreach (QNetworkInterface ni, QNetworkInterface::allInterfaces()) {
    // Get the first non-loopback MAC Address which is up & running
    if (ni.isValid() && !(ni.flags() & QNetworkInterface::IsLoopBack) &&
        ni.flags() & QNetworkInterface::IsRunning) {
      netInterface = ni;
    }
  }

  return netInterface;
}

QString IPv4Address = "";
QString getIPv4Address() {
  ENTER_FUNC

  if (IPv4Address.length() == 0) {
    QNetworkInterface netInterface = getNetworkInterface();
    if (netInterface.isValid()) {
      foreach (QNetworkAddressEntry addressEntry,
               netInterface.addressEntries()) {
        if (addressEntry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
          IPv4Address = addressEntry.ip().toString();
        }
      }
    }
  }
  LOG_SETTING("IPv4 Address", IPv4Address);

  LEAVE_FUNC
  return IPv4Address;
}

QString MACAddress = "";
QString getMACAddress() {
  ENTER_FUNC

  if (MACAddress.length() == 0) {
    QNetworkInterface netInterface = getNetworkInterface();
    if (netInterface.isValid()) {
      MACAddress = netInterface.hardwareAddress();
    }
  }
  LOG_SETTING("MAC Address", MACAddress);

  LEAVE_FUNC
  return MACAddress;
}

QString hostname = "";
QString getHostname() {
  ENTER_FUNC

  if (hostname.length() == 0) {
    hostname = QHostInfo::localHostName();
  }
  LOG_SETTING("Hostname", hostname);

  LEAVE_FUNC
  return hostname;
}
