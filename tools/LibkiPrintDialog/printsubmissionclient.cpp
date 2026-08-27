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

#include "printsubmissionclient.h"

#include <QDataStream>
#include <QDebug>
#include <QLocalSocket>

PrintSubmissionClient::PrintSubmissionClient() { socket = new QLocalSocket(); }

PrintSubmissionClient::~PrintSubmissionClient() {
  if (socket->state() == QLocalSocket::ConnectedState)
    socket->disconnectFromServer();

  delete socket;
}

bool PrintSubmissionClient::connectToServer(int timeout) {
  errorString.clear();

  if (socket->state() == QLocalSocket::ConnectedState) return true;

  socket->connectToServer(LIBKI_PRINT_SERVER_NAME);

  if (!socket->waitForConnected(timeout)) {
    errorString = socket->errorString();
    return false;
  }

  return true;
}

bool PrintSubmissionClient::submitPrint(const SubmitPrintRequest &request) {
  if (!connectToServer()) return false;

  QDataStream out(socket);
  out.setVersion(QDataStream::Qt_5_5);

  out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
  out << (quint32)PrintMessage_SubmitPrintRequest;
  out << request;

  socket->flush();

  return waitForReply();
}

bool PrintSubmissionClient::getPrintInfo(const PrintInfoRequest &request,
                                         PrintInfoReply &reply) {
  if (!connectToServer()) return false;

  QDataStream out(socket);
  out.setVersion(QDataStream::Qt_5_5);

  out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
  out << (quint32)PrintMessage_GetPrintInfoRequest;
  out << request;

  socket->flush();

  if (!socket->waitForReadyRead(3000)) {
    errorString = socket->errorString();

    //
    // Don't immediately fail if the server closed the
    // connection after writing.
    //
    if (socket->bytesAvailable() == 0) return false;
  }

  QDataStream in(socket);
  in.setVersion(QDataStream::Qt_5_5);

  quint32 version;
  quint32 message;

  in >> version;
  in >> message;

  if (version != LIBKI_PRINT_PROTOCOL_VERSION) {
    errorString = "Protocol version mismatch.";
    return false;
  }

  if (message != PrintMessage_GetPrintInfoReply) {
    errorString = "Unexpected reply from server.";
    return false;
  }

  in >> reply;

  if (!reply.success) {
    errorString = reply.error;
    return false;
  }

  return true;
}

bool PrintSubmissionClient::waitForReply() {
  if (!socket->waitForReadyRead(3000)) {
    errorString = socket->errorString();
    return false;
  }

  QDataStream in(socket);
  in.setVersion(QDataStream::Qt_5_5);

  quint32 version;
  quint32 status;
  QString message;

  in >> version;
  in >> status;
  in >> message;

  if (version != LIBKI_PRINT_PROTOCOL_VERSION) {
    errorString = "Protocol version mismatch.";
    return false;
  }

  if (status != 0) {
    errorString = message;
    return false;
  }

  return true;
}

QString PrintSubmissionClient::lastError() const { return errorString; }
