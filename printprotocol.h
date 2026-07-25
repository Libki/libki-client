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
#include <QDataStream>

static const quint32 LIBKI_PRINT_PROTOCOL_VERSION = 1;
static const char LIBKI_PRINT_SERVER_NAME[] = "LibkiPrintServer";

enum PrintMessage {
  PrintMessage_SubmitPrint = 1
};

struct SubmitPrintRequest
{
  QString filename;
  QString printer;
  int copies;
  int pageCount;
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

inline QDataStream &operator>>(QDataStream &in,
                               SubmitPrintRequest &r)
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

#endif
