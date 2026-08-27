/*
 * Copyright 2026 Ian Walls <ian@bywatersolutions.com>
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

#ifndef PRINTSUBMISSIONCLIENT_H
#define PRINTSUBMISSIONCLIENT_H

#include <QString>
#include <QStringList>

#include "../../printprotocol.h"

class QLocalSocket;

class PrintSubmissionClient {
 public:
  PrintSubmissionClient();
  ~PrintSubmissionClient();

  bool connectToServer(int timeout = 3000);

  bool submitPrint(const SubmitPrintRequest &request);

  bool getPrintInfo(const PrintInfoRequest &request, PrintInfoReply &reply);

  QString lastError() const;

 private:
  bool waitForReply();

  QLocalSocket *socket;
  QString errorString;
};

#endif  // PRINTSUBMISSIONCLIENT_H
