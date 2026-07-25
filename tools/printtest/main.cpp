#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QLocalSocket>

#include "../../printprotocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SubmitPrintRequest request;
    QString error;

    if (!SubmitPrintRequest::fromArguments(
            app.arguments(),
            request,
            error))
    {
        qCritical() << error;
        qCritical().noquote()
            << SubmitPrintRequest::usage(app.arguments()[0]);
        return 1;
    }

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

    QDataStream out(&socket);
    out.setVersion(QDataStream::Qt_5_5);

    out << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
    out << (quint32)PrintMessage_SubmitPrint;
    out << request;

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

    return (status == PrintStatus_Ok) ? 0 : 1;
}
