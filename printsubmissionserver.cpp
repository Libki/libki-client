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

#include "printsubmissionserver.h"

#include <QDataStream>
#include <QDebug>
#include <QLocalSocket>

#include "log.h"
#include "printprotocol.h"

PrintSubmissionServer::PrintSubmissionServer(QObject *parent)
    : QObject(parent) {
  server = new QLocalServer(this);

  connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

bool PrintSubmissionServer::start() {
  QLocalServer::removeServer(LIBKI_PRINT_SERVER_NAME);

  if (!server->listen(LIBKI_PRINT_SERVER_NAME)) {
    log::error(QString("Unable to start PrintSubmissionServer: %1")
                   .arg(server->errorString()));
    return false;
  }

  log::debug(QString("PrintSubmissionServer listening on %1")
                 .arg(LIBKI_PRINT_SERVER_NAME));

  return true;
}
void PrintSubmissionServer::newConnection() {
  while (server->hasPendingConnections()) {
    QLocalSocket *socket = server->nextPendingConnection();

    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
  }
}

void PrintSubmissionServer::socketReadyRead() {
  QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());

  if (!socket) return;

  QString error;

  bool finished = processSocket(socket, error);

  if (!error.isEmpty()) log::warn(error);

  if (finished && socket->state() == QLocalSocket::ConnectedState) {
    socket->disconnectFromServer();
  }
}

bool PrintSubmissionServer::processSocket(QLocalSocket *socket,
                                          QString &error) {
  if (!socket) return true;

  if (socket->bytesAvailable() < sizeof(quint32) * 2) return false;

  QDataStream stream(socket);
  stream.setVersion(QDataStream::Qt_5_5);

  quint32 version;
  quint32 message;

  stream >> version;
  stream >> message;

  if (version != LIBKI_PRINT_PROTOCOL_VERSION) {
    error = tr("Protocol version mismatch.");
    return true;
  }

  switch (message) {
    case PrintMessage_SubmitPrintRequest: {
      SubmitPrintRequest request;

      stream >> request;

      emit submitPrintRequested(request);

      QDataStream out(socket);
      out.setVersion(QDataStream::Qt_5_5);

      out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
      out << (quint32)PrintStatus_Ok;
      out << QString("OK");

      socket->flush();
      socket->waitForBytesWritten(1000);

      return true;
    }
    case PrintMessage_GetPrintInfoRequest: {
      PrintInfoRequest request;

      stream >> request;

      emit printInfoRequested(request, socket);

      return false;
    }

    default:
      log::warn("Unknown IPC message");
      return true;
  }

  QDataStream out(socket);
  out.setVersion(QDataStream::Qt_5_5);

  out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
  out << (quint32)PrintStatus_Ok;
  out << QString("OK");

  socket->flush();
  socket->waitForBytesWritten(1000);

  socket->disconnectFromServer();
  return true;
}

void PrintSubmissionServer::sendPrintInfoReply(QLocalSocket *socket,
                                               const PrintInfoReply &reply) {
  if (!socket) return;

  QDataStream stream(socket);

  stream << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
  stream << (quint32)PrintMessage_GetPrintInfoReply;
  stream << reply;

  socket->flush();
  socket->disconnectFromServer();
}
