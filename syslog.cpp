#include "syslog.h"

#include <QUdpSocket>
#include <QDateTime>

Facility QStringToFacility(QString& facility_str) {
  if (facility_str == "local1") {
    return Local1;
  } else if (facility_str == "local2") {
    return Local2;
  } else if (facility_str == "local3") {
    return Local3;
  } else if (facility_str == "local4") {
    return Local4;
  } else if (facility_str == "local5") {
    return Local5;
  } else if (facility_str == "local6") {
    return Local6;
  } else if (facility_str == "local7") {
    return Local7;
  }

  return Local0;
}


Syslog::Syslog(QString server, quint16 port, Facility facility, QString hostname, QString appname, int procid) {
  this->address = QHostAddress(server);
  this->port = port;
  this->facility = facility;
  this->hostname = hostname;
  this->appname = appname;
  this->procid = procid;
}

bool Syslog::sendSyslog(Severity severity, QString message) {
  int priority = (this->facility * 8) + severity;

  QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-ddTHH:mm:ss.zzz"));

  QString syslog_header = QString("<%1>1 %2 %3 %4 %5 - -").arg(priority).arg(timestamp).arg(this->hostname).arg(this->appname).arg(this->procid);

  //RFC5424 6.4 Message BOM indicating UTF8 message
  QByteArray bom = "\xEF\xBB\xBF";

  //RFC5424 6.2 header must be seven bit ASCII
  QByteArray syslog_payload = syslog_header.toLatin1() + bom + message.toUtf8();

  QUdpSocket syslogSocket;
  qint64 status = syslogSocket.writeDatagram(syslog_payload, this->address, this->port);

  return status != syslog_payload.length();
}
