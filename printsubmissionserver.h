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

#ifndef PRINTSUBMISSIONSERVER_H
#define PRINTSUBMISSIONSERVER_H

#include <QObject>
#include <QLocalServer>
#include "printprotocol.h"

class QLocalSocket;

class PrintSubmissionServer : public QObject
{
    Q_OBJECT

public:
    explicit PrintSubmissionServer(QObject *parent = 0);

    bool start();

signals:
  void submitPrintRequested(
    const SubmitPrintRequest &request);

private slots:
 void newConnection();
 void socketReadyRead();

private:
    bool processSocket(QLocalSocket *socket, QString &errorMessage);

    QLocalServer *server;
};

#endif
