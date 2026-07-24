#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QLocalSocket>

#include "../../printprotocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QLocalSocket socket;

    qDebug() << "Connecting to"
             << LIBKI_PRINT_SERVER_NAME;

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

    stream << (quint32)LIBKI_PRINT_PROTOCOL_VERSION;
    stream << (quint32)PrintMessage_SubmitPrint;

    stream << QString("/tmp/test.pdf");
    stream << QString("Color Printer");
    stream << (qint32)2;
    stream << (qint32)17;

    qDebug() << "Test Message sent.";

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

    socket.waitForDisconnected(1000);

//    return 0;
}
