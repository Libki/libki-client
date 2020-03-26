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

#include <QSettings>
#include <QLocale>
#include "utils.h"

QString getLabel(QString labelcode) {
  QSettings settings;
  settings.setIniCodec("UTF-8");

  QString locale = QLocale::system().name();
  QString label = QString();

  if (!settings.value("labels-" + locale + "/" + labelcode).toString().isEmpty()) {
    label = settings.value("labels-" + locale + "/" + labelcode).toString();
  } else if (!settings.value("labels-" + locale.left(locale.indexOf('_')) + "/" + labelcode).toString().isEmpty()) {
    label = settings.value("labels-" + locale.left(locale.indexOf('_')) + "/" + labelcode).toString();
  } else if (!settings.value("labels/" + labelcode).toString().isEmpty()) {
    label = settings.value("labels/" + labelcode).toString();
  }

  return label;
}
