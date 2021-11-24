#ifndef LOGUTILS_H
#define LOGUTILS_H

#define LOGSIZE 1024 * 100  // log size in bytes
#define LOGFILES 20

#include <QDate>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QTime>

namespace LogUtils {
bool initLogging();
void myMessageHandler(QtMsgType type, const QMessageLogContext& context,
                      const QString& msg);
}  // namespace LogUtils

#endif  // LOGUTILS_H
