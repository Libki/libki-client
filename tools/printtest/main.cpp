#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QLocalSocket>

#include "../../printprotocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();

    if (args.size() < 3) {
        qCritical()
            << "Usage:"
            << args[0]
            << "<filename> <printer> [copies] [pages]";
        return 1;
    }

    SubmitPrintRequest request;

    request.filename = args[1];
    request.printer  = args[2];
    request.copies   = (args.size() >= 4) ? args[3].toInt() : 1;
    request.pageCount = (args.size() >= 5) ? args[4].toInt() : 0;

    qDebug() << "Connecting to" << LIBKI_PRINT_SERVER_NAME;

    QLocalSocket socket;

    socket.connectToServer(LIBKI_PRINT_SERVER_NAME);

    if (!socket.waitForConnected(3000)) {
        qCritical()
            << "Failed to connect:"
            << socket.errorString();
        return 1;
    }

    qDebug() << "Connected.";

    QDataStream stream(&socket);
    stream.setVersion(QDataStream::Qt_5_5);

    qDebug() << "Sending:";
    qDebug() << "  file    =" << request.filename;
    qDebug() << "  printer =" << request.printer;
    qDebug() << "  copies  =" << request.copies;
    qDebug() << "  pages   =" << request.pageCount;

    stream << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
    stream << (quint32)PrintMessage_SubmitPrint;
    stream << request;

    socket.flush();

    if (!socket.waitForReadyRead(3000)) {
        qCritical()
            << "Timed out waiting for reply:"
            << socket.errorString();
        return 1;
    }

    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_5_5);

    quint32 version;
    quint32 status;
    QString message;

    in >> version;
    in >> status;
    in >> message;

    qDebug() << "Server replied:";
    qDebug() << "  version =" << version;
    qDebug() << "  status  =" << status;
    qDebug() << "  message =" << message;

    socket.disconnectFromServer();

    return (status == 0) ? 0 : 1;
}
