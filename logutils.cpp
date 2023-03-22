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

namespace LogUtils {
static QString logFileName;
static QString logFolderName;
static QFile* logFile;

void initLogFileName() {
  qDebug("ENTER LogUtils::iniLogFileName");

  // Check environment variable for logs directory
  QString path = qgetenv("LIBKI_LOGS_DIR");
  qDebug() << "LOGS ENV VAR: " << path;

  // Next, check the user level registry ( on Windows )
  if ( path.isEmpty() ) {
      QSettings settings("HKEY_CURRENT_USER\\Software\\Libki", QSettings::NativeFormat);
      path = settings.value("logs_dir").toString();
      qDebug() << "HKCU LOGS DIR: " << path;
  }

  // Next, check the machine level registry ( on Windows )
  if ( path.isEmpty() ) {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Libki", QSettings::NativeFormat);
      path = settings.value("logs_dir").toString();
      qDebug() << "HKLM LOGS DIR: " << path;
  }

  // Finally, default to AppDataLocation
  if ( path.isEmpty() ) {
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
  // Create folder for logfiles if not exists
  if (!QDir(logFolderName).exists()) {
    qDebug() << "Creating directory " << logFolderName;
    QDir().mkdir(logFolderName);
  }

  deleteOldLogs();    // delete old log files
  initLogFileName();  // create the logfile name

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

  QString levelText;
  switch (type) {
    case QtDebugMsg:
      levelText = "Debug";
      break;
    case QtInfoMsg:
      levelText = "Info";
      break;
    case QtWarningMsg:
      levelText = "Warning";
      break;
    case QtCriticalMsg:
      levelText = "Critical";
      break;
    case QtFatalMsg:
      levelText = "Fatal";
      break;
  }

  QString text = QString("%3 [%1] %2")
                     .arg(levelText)
                     .arg(message)
                     .arg(QDateTime::currentDateTime().toString(Qt::ISODate));

  // Output to console
  Qt::endl(QTextStream(stdout) << text);

  // Output to log file
  QTextStream ts(logFile);
  Qt::endl(ts << text);
}
}  // namespace LogUtils
