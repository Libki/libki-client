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

#ifndef PRINTPROTOCOL_H
#define PRINTPROTOCOL_H

#include <QString>
#include <QStringList>
#include <QDataStream>

static const quint32 LIBKI_PRINT_PROTOCOL_VERSION = 1;
static const char LIBKI_PRINT_SERVER_NAME[] = "LibkiPrintServer";

enum PrintMessage {
  PrintMessage_GetPrintInfoRequest = 1,
  PrintMessage_GetPrintInfoReply   = 2,
  PrintMessage_SubmitPrintRequest  = 3,
  PrintMessage_SubmitPrintReply    = 4
};

enum PrintStatus {
  PrintStatus_Ok = 0,
  PrintStatus_Error = 1
};

struct PrintInfoRequest
{
  QString printer;
  int pageCount;
  int copies;
};

struct PrintInfoReply
{
  bool success;
  QString currency;
  double costPerPage;
  double estimatedCost;
  double availableFunds;
  double gratisBalance;
  QString gratisMethod;
  double remainingBalance;
  bool canPrint;
  QString error;
};

struct SubmitPrintRequest
{
  SubmitPrintRequest()
      : copies(1),
        pageCount(0)
  {
  }

  QString filename;
  QString printer;
  int copies;
  int pageCount;

  static bool fromArguments(const QStringList &arguments,
                            SubmitPrintRequest &request,
                            QString &error);

  static QString usage(const QString &programName);
};

inline QDataStream &operator<<(QDataStream &out,
                               const SubmitPrintRequest &r)
{
  out << r.filename
      << r.printer
      << qint32(r.copies)
      << qint32(r.pageCount);

  return out;
}

inline QDataStream &operator>>(QDataStream &in, SubmitPrintRequest &r)
{
  qint32 copies;
  qint32 pages;

  in >> r.filename;
  in >> r.printer;
  in >> copies;
  in >> pages;

  r.copies = copies;
  r.pageCount = pages;

  return in;
}

inline QDataStream &operator<<(QDataStream &out,
                               const PrintInfoRequest &request)
{
  out << request.printer;
  out << request.pageCount;
  out << request.copies;
  return out;
}

inline QDataStream &operator>>(QDataStream &in,
                               PrintInfoRequest &request)
{
  in >> request.printer;
  in >> request.pageCount;
  in >> request.copies;
  return in;
}

inline QDataStream &operator<<(QDataStream &out,
                               const PrintInfoReply &reply)
{
  out << reply.success;
  out << reply.currency;
  out << reply.costPerPage;
  out << reply.estimatedCost;
  out << reply.availableFunds;
  out << reply.gratisBalance;
  out << reply.gratisMethod;
  out << reply.remainingBalance;
  out << reply.canPrint;
  out << reply.error;
  return out;
}

inline QDataStream &operator>>(QDataStream &in,
                               PrintInfoReply &reply)
{
  in >> reply.success;
  in >> reply.currency;
  in >> reply.costPerPage;
  in >> reply.estimatedCost;
  in >> reply.availableFunds;
  in >> reply.gratisBalance;
  in >> reply.gratisMethod;
  in >> reply.remainingBalance;
  in >> reply.canPrint;
  in >> reply.error;
  return in;
}

inline QString SubmitPrintRequest::usage(const QString &programName)
{
  return QString("%1 <filename> <printer> <copies> <pages>")
      .arg(programName);
}

inline bool SubmitPrintRequest::fromArguments(
    const QStringList &arguments,
    SubmitPrintRequest &request,
    QString &error)
{
  if (arguments.size() != 5) {
    error = QString("Usage: %1")
                .arg(usage(arguments.value(0, "LibkiPrintDialog")));
    return false;
  }

  request.filename = arguments[1];
  request.printer  = arguments[2];

  bool ok = false;

  request.copies = arguments[3].toInt(&ok);
  if (!ok || request.copies < 1) {
    error = "Invalid copies value.";
    return false;
  }

  request.pageCount = arguments[4].toInt(&ok);
  if (!ok || request.pageCount < 1) {
    error = "Invalid page count.";
    return false;
  }

  return true;
}

#endif
