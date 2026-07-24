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
  socket->waitForBytesWritten(1000);
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

    qDebug() << "Received version =" << version;
    qDebug() << "Received message =" << message;
    qDebug() << "Expected version =" << LIBKI_PRINT_PROTOCOL_VERSION;

    switch (message) {

    case PrintMessage_SubmitPrint:
    {
        QString filename;
        QString printer;
        qint32 copies;
        qint32 pages;

        stream >> filename;
        stream >> printer;
        stream >> copies;
        stream >> pages;

        qDebug()
            << "Print request:"
            << filename
            << printer
            << copies
            << pages;

        emit submitPrintRequested(
            filename,
            printer,
            copies,
            pages);

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
