#include "logutils.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSettings>
#include <QStandardPaths>
#include <QTime>
#include <iostream>

#include "syslog.h"
#include "utils.h"

namespace LogUtils {
static QString logFileName;
static QString logFolderName;
static QFile* logFile;
static LogUtils::LogLevel currentLogLevel = LogUtils::DebugLevel;
static Syslog* syslog;

LogUtils::LogLevel logLevel() { return currentLogLevel; }

void initLogFileName() {
  qDebug("ENTER LogUtils::iniLogFileName");

  // Check environment variable for logs directory
  QString path = qgetenv("LIBKI_LOGS_DIR");
  qDebug() << "LOGS ENV VAR: " << path;

  // Next, check the user level registry ( on Windows )
  if (path.isEmpty()) {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Libki",
                       QSettings::NativeFormat);
    path = settings.value("logs_dir").toString();
    qDebug() << "HKCU LOGS DIR: " << path;
  }

  // Next, check the machine level registry ( on Windows )
  if (path.isEmpty()) {
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Libki",
                       QSettings::NativeFormat);
    path = settings.value("logs_dir").toString();
    qDebug() << "HKLM LOGS DIR: " << path;
  }

  // Finally, default to AppDataLocation
  if (path.isEmpty()) {
    path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    qDebug() << "LOGS APP DATA LOCATION: " << path;
  }

  if (path.isEmpty()) qFatal("Cannot determine settings storage location");
  QDir d = QDir(path);
  QString appDataPath = d.absolutePath();

  logFolderName = appDataPath + "/logs";

  logFileName = QString(logFolderName + "/Log_%1__%2.txt")
                    .arg(QDate::currentDate().toString("yyyy_MM_dd"))
                    .arg(QTime::currentTime().toString("hh_mm_ss_zzz"));

  qDebug() << "LOG DIR NAME: " << logFolderName;
  qDebug() << "LOG FILE NAME: " << logFileName;

  d.mkpath(logFolderName);
  qDebug() << "LOG DIR EXISTS: " << QDir(logFolderName).exists();

  qDebug("LEAVE LogUtils::iniLogFileName");
}

void deleteOldLogs() {
  qDebug("ENTER LogUtils::deleteOldLogs");

  QDir dir;
  dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Time | QDir::Reversed);
  dir.setPath(logFolderName);

  QFileInfoList list = dir.entryInfoList();
  if (list.size() <= LOGFILES) {
    return;  // no files to delete
  } else {
    for (int i = 0; i < (list.size() - LOGFILES); i++) {
      QString path = list.at(i).absoluteFilePath();
      QFile file(path);
      file.remove();
    }
  }

  qDebug("LEAVE LogUtils::deleteOldLogs");
}

bool initLogging() {
  qDebug("ENTER LogUtils::initLogging");

  QSettings settings;

  QString level =
      settings.value("logging/level", "debug").toString().trimmed().toLower();

  if (level == "debug")
    currentLogLevel = DebugLevel;
  else if (level == "warning")
    currentLogLevel = WarningLevel;
  else if (level == "error")
    currentLogLevel = ErrorLevel;
  else if (level == "off")
    currentLogLevel = OffLevel;
  else
    currentLogLevel = DebugLevel;

  fprintf(stderr, "Configured log level = %d\n", (int)currentLogLevel);

  // Create folder for logfiles if not exists
  if (!QDir(logFolderName).exists()) {
    qDebug() << "Creating directory " << logFolderName;
    QDir().mkdir(logFolderName);
  }

  deleteOldLogs();    // delete old log files
  initLogFileName();  // create the logfile name

  QString enable_syslog =
      settings.value("logging/enable_syslog").toString().trimmed().toLower();

  qDebug() << "ENABLE SYSLOG: " << enable_syslog;

  if (enable_syslog == "yes") {
    qDebug("Enabling syslog.");
    QString syslog_server = settings.value("logging/syslog_server", "localhost")
                                .toString()
                                .trimmed()
                                .toLower();

    qDebug() << "SYSLOG SERVER: " << syslog_server;

    bool okay = false;
    quint16 syslog_port =
        settings.value("logging/syslog_port", "514").toUInt(&okay);

    qDebug() << "SYSLOG PORT: " << syslog_port;

    QString syslog_facility =
        settings.value("logging/syslog_facility", "local0")
            .toString()
            .trimmed()
            .toLower();
    qDebug() << "SYSLOG FACILITY: " << syslog_facility;

    QString syslog_hostname =
        settings.value("logging/syslog_hostname", getClientName())
            .toString()
            .trimmed()
            .toLower();
    qDebug() << "SYSLOG HOSTNAME: " << syslog_hostname;

    QString syslog_appname =
        settings.value("logging/syslog_appname", "libkiclient")
            .toString()
            .trimmed()
            .toLower();
    qDebug() << "SYSLOG APPNAME: " << syslog_appname;

    if (!okay) {
      qWarning("Couldn't parse syslog port.");
    } else {
      syslog = new Syslog(syslog_server, syslog_port,
                          QStringToFacility(syslog_facility), syslog_hostname,
                          syslog_appname, QCoreApplication::applicationPid());
    }
  } else {
    syslog = NULL;
  }

  logFile = new QFile(logFileName);
  if (logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
    qInstallMessageHandler(LogUtils::myMessageHandler);

    qDebug("LEAVE LogUtils::initLogging - Return true");
    return true;
  } else {
    qDebug("LEAVE LogUtils::initLogging - Return false");
    return false;
  }
}

void myMessageHandler(QtMsgType type, const QMessageLogContext& context,
                      const QString& message) {
  // check file size and if needed create new log!
  {
    if (logFile->size() > LOGSIZE)  // check current log size
    {
      deleteOldLogs();
      initLogFileName();
    }
  }

  switch (type) {
    case QtDebugMsg:
      if (currentLogLevel > DebugLevel) return;
      break;

    case QtWarningMsg:
      if (currentLogLevel > WarningLevel) return;
      break;

    case QtCriticalMsg:
      if (currentLogLevel > ErrorLevel) return;
      break;

    case QtFatalMsg:
      break;

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtInfoMsg:
      if (currentLogLevel > DebugLevel) return;
      break;
#endif
  }

  Severity syslog_severity = Debug;
  QString levelText;
  switch (type) {
    case QtDebugMsg:
      levelText = "Debug";
      break;
    case QtInfoMsg:
      levelText = "Info";
      syslog_severity = Informational;
      break;
    case QtWarningMsg:
      levelText = "Warning";
      syslog_severity = Warning;
      break;
    case QtCriticalMsg:
      levelText = "Critical";
      syslog_severity = Critical;
      break;
    case QtFatalMsg:
      levelText = "Fatal";
      syslog_severity = Critical;
      break;
  }

  QString text = QString("%3 [%1] %2")
                     .arg(levelText)
                     .arg(message)
                     .arg(QDateTime::currentDateTime().toString(Qt::ISODate));

  // Output to console
  QTextStream(stdout) << text << endl;

  // Output to log file
  QTextStream ts(logFile);
  ts << text << endl;

  // Output to syslog, if enabled
  if (syslog != NULL) {
    if (syslog->sendSyslog(syslog_severity, message)) {
      QTextStream(stdout) << "Failed to write to syslog." << endl;
      ts << "Failed to write to syslog." << endl;
    }
  }
}

}  // namespace LogUtils
