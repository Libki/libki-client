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

#include "printprotocol.h"

PrintSubmissionServer::PrintSubmissionServer(QObject *parent) : QObject(parent) {
    server = new QLocalServer(this);

    connect(server,
            SIGNAL(newConnection()),
            this,
            SLOT(newConnection()));
}

bool PrintSubmissionServer::start() {
    QLocalServer::removeServer(LIBKI_PRINT_SERVER_NAME);

    if (!server->listen(LIBKI_PRINT_SERVER_NAME)) {
        qWarning() << "Unable to start PrintSubmissionServer:"
                   << server->errorString();
        return false;
    }

    qDebug() << "PrintSubmissionServer listening on"
             << LIBKI_PRINT_SERVER_NAME;

    return true;
}
void PrintSubmissionServer::newConnection() {
    while (server->hasPendingConnections()) {

        QLocalSocket *socket =
            server->nextPendingConnection();

        connect(socket,
                SIGNAL(readyRead()),
                this,
                SLOT(socketReadyRead()));

        connect(socket,
                SIGNAL(disconnected()),
                socket,
                SLOT(deleteLater()));
    }
}

void PrintSubmissionServer::socketReadyRead()
{
  QLocalSocket *socket =
      qobject_cast<QLocalSocket *>(sender());

  if (!socket)
    return;

  qDebug() << "bytes available:" << socket->bytesAvailable();

  QString error;

  bool ok = processSocket(socket, error);

  QDataStream out(socket);
  out.setVersion(QDataStream::Qt_5_5);

  out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
  out << (quint32)(ok ? 0 : 1);
  out << (ok ? QString("OK") : error);

  socket->flush();
  socket->disconnectFromServer();
}

bool PrintSubmissionServer::processSocket(QLocalSocket *socket, QString &errorMessage) {
    if (socket->bytesAvailable() < 8)
      return 1;
    QDataStream stream(socket);

    quint32 version;
    quint32 message;

    stream.setVersion(QDataStream::Qt_5_5);

    stream >> version;
    stream >> message;

    switch (message) {

    case PrintMessage_SubmitPrint:
    {
        SubmitPrintRequest request;

        stream >> request;

        qDebug()
            << "Print request:"
            << request.filename
            << request.printer
            << request.copies
            << request.pageCount;

        emit submitPrintRequested(request);

        break;
    }

    default:
        qWarning()
            << "Unknown IPC message";
    }

    QDataStream out(socket);
    out.setVersion(QDataStream::Qt_5_5);

    out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
    out << (quint32)0;
    out << QString("OK");

    socket->flush();
    socket->waitForBytesWritten(1000);

    socket->disconnectFromServer();
    return 0;
}
