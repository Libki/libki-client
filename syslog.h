#ifndef SYSLOG_H
#define SYSLOG_H

#include <QApplication>
#include <QHostAddress>
#include <QObject>

typedef enum Facility {
  Local0 = 16,
  Local1 = 17,
  Local2 = 18,
  Local3 = 19,
  Local4 = 20,
  Local5 = 21,
  Local6 = 22,
  Local7 = 23,
} Facility;

typedef enum Severity {
  Emergency,
  Alert,
  Critical,
  Error,
  Warning,
  Notice,
  Informational,
  Debug
} Severity;

Facility QStringToFacility(QString& facility_str);

class Syslog : public QObject {
  Q_OBJECT
 private:
  Facility facility;
  QString hostname;
  QString appname;
  int procid;

  QHostAddress address;
  quint16 port;

 public:
  Syslog(QString server, quint16 port, Facility facility, QString hostname,
         QString appname, int procid);
  bool sendSyslog(Severity severity, QString message);
};

#endif  // SYSLOG_H
