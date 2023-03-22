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
#include <QtNetwork/QHostInfo>
#include <QSettings>

QString getLabel(QString labelcode) {
  qDebug("ENTER utils/getLabel");

  QSettings settings;
  //settings.setIniCodec("UTF-8");

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

  qDebug("LEAVE utils/getLabel");
  return label;
}

QString clientName = "";
QString getClientName() {
    qDebug("ENTER utils/getClientName");

    if ( clientName.length() == 0 ) {
        QSettings settings;
        //settings.setIniCodec("UTF-8");


        QString os_username;
#ifdef Q_OS_WIN
        os_username = getenv("USERNAME");
#endif  // ifdef Q_OS_WIN
#ifdef Q_OS_UNIX
        os_username = getenv("USER");
#endif  // ifdef Q_OS_UNIX

        clientName = settings.value("node/name").toString();

        qDebug() << "OS USERNAME: " << os_username;
        qDebug() << "CONFIG NODE NAME: " << clientName;

        if (clientName == "OS_USERNAME") {
          clientName = os_username;
        }

        // Fail over to hostname if node name isn't defined.
        if (clientName.isEmpty()) {
          QHostInfo hostInfo;
          hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
          clientName = QHostInfo::localHostName();
        }
        qDebug() << "NODE NAME: " << clientName;
    }

    qDebug("LEAVE utils/getClientName");
    return clientName;
}
