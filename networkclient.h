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

#include <QObject>
#include <QHash>

namespace NetworkError {
  enum Enum {
    SERVER_GONE,
    BAD_LOGIN,
    NO_TIME
  };
}


class NetworkClient : public QObject {
  Q_OBJECT

  public:
    NetworkClient();

    bool attemptLogin( QString username, QString password, int & error );
};

#endif // NETWORKCLIENT_H
