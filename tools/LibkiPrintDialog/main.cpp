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


#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include <QDebug>

#include "printdialog.h"
#include "printsubmissionclient.h"

static void usage(const QString &program)
{
    qCritical()
        << "Usage:"
        << program
        << "<printer> <copies> <pages> <filename>";
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SubmitPrintRequest request;
    QString error;

    if (!SubmitPrintRequest::fromArguments(app.arguments(), request, error)) {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Invalid Arguments"),
            error);
        return 1;
    }

    PrintSubmissionClient client;

    PrintInfoRequest infoRequest;
    infoRequest.printer = request.printer;
    infoRequest.pageCount = request.pageCount;
    infoRequest.copies = request.copies;

    PrintInfoReply info;

    if (!client.getPrintInfo(infoRequest, info)) {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Unable to retrieve print pricing"),
            client.lastError());
        return 1;
    }

    PrintDialog dialog(request, info);

    if (dialog.exec() != QDialog::Accepted)
        return 0;

    if (!client.submitPrint(request)) {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Print Failed"),
            client.lastError());
        return 1;
    }

    return 0;
}
