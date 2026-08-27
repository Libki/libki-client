//
// Created by kkuhn on 8/20/2026.
//

#ifndef LIBKI_CLIENT_LOGSDATA_H
#define LIBKI_CLIENT_LOGSDATA_H

#include <QMap>
#include <QNetworkReply>

#include "printprotocol.h"

class SData {
  QString m_sdid;
  QMap<QString, QString> m_sdata;

 public:
  SData(const QString& sdid);

  static SData new_nil();
  static SData new_authresult(bool, const QString& error);
  static SData new_authrequest(const QString& username, const QString& action,
                               const QString& server);
  static SData new_bypassattempt(const QString& attempt_hash,
                                 const QString& stored_hash, bool allowed);
  static SData new_checkinternet(const QString& server);
  static SData new_funcframe(const QString& func_name);
  static SData new_node(const QString& server, const QString& node_name,
                        const QString& version, const QString& age_limit);
  static SData new_nodepower(const QString& action);
  static SData new_printer(const QString& printer_name,
                           const QString& printer_path);
  static SData new_printjob(const SubmitPrintRequest& request);
  static SData new_reservation(const QString& reservation_for);
  static SData new_serverreply(const QNetworkReply& reply);
  static SData new_settingvalue(const QString& key, const QString& value);
  static SData new_shell(const QString& shell);
  static SData new_usermatch(const QString& user, const QString& stored_user,
                             bool allowed);
  static SData new_printerquery(const QString& printer);
  static SData new_printerresult(const PrintInfoReply& reply,
                                 const QString& error);
  static SData new_printerresult(const QString& error);

  void add_sdparam(const QString& param_name, const QString(&param_value));
  QString serialize_sdata() const;
};

#endif  // LIBKI_CLIENT_LOGSDATA_H
