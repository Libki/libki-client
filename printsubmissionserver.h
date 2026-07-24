#ifndef PRINTSUBMISSIONSERVER_H
#define PRINTSUBMISSIONSERVER_H

#include <QObject>
#include <QLocalServer>

class QLocalSocket;

class PrintSubmissionServer : public QObject
{
    Q_OBJECT

public:
    explicit PrintSubmissionServer(QObject *parent = 0);

    bool start();

signals:
    void submitPrintRequested(
        QString filename,
        QString printer,
        int copies,
        int pageCount);

private slots:
 void newConnection();
 void socketReadyRead();

private:
    bool processSocket(QLocalSocket *socket, QString &errorMessage);

    QLocalServer *server;
};

#endif
