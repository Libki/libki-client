//
// Created by kkuhn on 8/20/2026.
//

#ifndef LIBKI_CLIENT_LOG_H
#define LIBKI_CLIENT_LOG_H

#include <QString>

#include "logsdata.h"

#define NIL_ID "-"
#define AUTHRESULT_ID "AUTHRESULT"
#define BYPASS_ID "BYPASS"
#define CHECKINTERNET_ID "CHECKINTERNET"
#define CHECKINTERNETREPLY_ID "CHECKINTERNET_REPLY"
#define CLOSEEVENT_ID "CLOSEEVENT"
#define ENTERFUNC_ID "ENTERFUNC"
#define LEAVEFUNC_ID "LEAVEFUNC"
#define LOGIN_ID "LOGIN"
#define LOGOUT_ID "LOGOUT"
#define NODELOCK_ID "NODELOCK"
#define NODEPOWER_ID "NODEPOWER"
#define NODEUNLOCK_ID "NODEUNLOCK"
#define ONLYRUNFOR_ID "ONLYRUNFOR"
#define ONLYSTOPFOR_ID "ONLYSTOPFOR"
#define PRINTERFOUND_ID "PRINTERFOUND"
#define PRINTJOBRESULT_ID "PRINTJOBRESULT"
#define PRINTJOBSEND_ID "PRINTJOBFSEND"
#define REGISTERNODE_ID "REGISTERNODE"
#define REGISTERNODEREPLY_ID "REGISTERNODEREPLY"
#define RESERVATIONACK_ID "RESERVATIONACK"
#define SETTING_ID "SETTING"
#define STARTSHELL_ID "STARTSHELL"
#define QUERYUSER_ID "QUERYUSER"
#define QUERYPRINTER_ID "QUERYPRINTER"
#define QUERYPRINTERRESULT_ID "QUERYPRINTERRESULT"

#define ENTER_FUNC                                                       \
  log::debug(QString("ENTER %1").arg(__PRETTY_FUNCTION__), ENTERFUNC_ID, \
             SData::new_funcframe(__PRETTY_FUNCTION__));

#define LEAVE_FUNC                                                       \
  log::debug(QString("LEAVE %1").arg(__PRETTY_FUNCTION__), LEAVEFUNC_ID, \
             SData::new_funcframe(__PRETTY_FUNCTION__));

#define LOG_SETTING(key, value) \
  log::debug("", SETTING_ID, SData::new_settingvalue((key), (value)))

namespace log {

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

void log(Severity severity, const QString& msgid, const SData& sdata,
         const QString& message);

inline void debug(const QString& message = "", const QString& msgid = NIL_ID,
                  const SData& sdata = SData::new_nil()) {
  log(Debug, msgid, sdata, message);
}

inline void info(const QString& message = "", const QString& msgid = NIL_ID,
                 const SData& sdata = SData::new_nil()) {
  log(Informational, msgid, sdata, message);
}

inline void notice(const QString& message = "", const QString& msgid = NIL_ID,
                   const SData& sdata = SData::new_nil()) {
  log(Notice, msgid, sdata, message);
}

inline void warn(const QString& message = "", const QString& msgid = NIL_ID,
                 const SData& sdata = SData::new_nil()) {
  log(Warning, msgid, sdata, message);
}

inline void error(const QString& message = "", const QString& msgid = NIL_ID,
                  const SData& sdata = SData::new_nil()) {
  log(Error, msgid, sdata, message);
}

inline void critical(const QString& message = "", const QString& msgid = NIL_ID,
                     const SData& sdata = SData::new_nil()) {
  log(Critical, msgid, sdata, message);
}

inline void alert(const QString& message, const QString& msgid = NIL_ID,
                  const SData& sdata = SData::new_nil()) {
  log(Alert, msgid, sdata, message);
}

inline void emergency(const QString& message = "",
                      const QString& msgid = NIL_ID,
                      const SData& sdata = SData::new_nil()) {
  log(Emergency, msgid, sdata, message);
}

class log {};

}  // namespace log

#endif  // LIBKI_CLIENT_LOG_H
