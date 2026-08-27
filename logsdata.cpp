//
// Created by kkuhn on 8/20/2026.
//

#include "logsdata.h"
SData::SData(const QString& sdid) {
  this->m_sdid = sdid;
}

SData SData::new_nil() {
  SData sdata("Nil@32473");
  return sdata;
}

SData SData::new_authresult(bool authenticated, const QString& error) {
  SData sdata("AuthResult@32473");
  sdata.add_sdparam("authenticated", authenticated ? "yes" : "no");
  sdata.add_sdparam("error", error);
  return sdata;
}
SData SData::new_authrequest(const QString& username, const QString& action,
                             const QString& server) {
  SData sdata("AuthRequest@32473");
  sdata.add_sdparam("username", username);
  sdata.add_sdparam("action", action);
  sdata.add_sdparam("server", server);
  return sdata;
}

SData SData::new_bypassattempt(const QString& attempt_hash,
                               const QString& stored_hash, bool allowed) {
  SData sdata("BypassAttempt@32473");
  sdata.add_sdparam("attemptHash", attempt_hash);
  sdata.add_sdparam("storedHash", stored_hash);
  sdata.add_sdparam("allowed", allowed ? "yes" : "no");
  return sdata;
}

SData SData::new_checkinternet(const QString& server) {
  SData sdata("CheckInternet@32473");
  sdata.add_sdparam("server", server);
  return sdata;
}

SData SData::new_funcframe(const QString& func_name) {
  SData sdata("FuncFrame@32473");
  sdata.add_sdparam("funcName", func_name);
  return sdata;
}

SData SData::new_node(const QString& server, const QString& node_name,
                      const QString& version, const QString& age_limit) {
  SData sdata("Node@32473");
  sdata.add_sdparam("server", server);
  sdata.add_sdparam("nodeName", node_name);
  sdata.add_sdparam("version", version);
  sdata.add_sdparam("ageLimit", age_limit);
  return sdata;
}

SData SData::new_nodepower(const QString& action) {
  SData sdata("NodePower@32473");
  sdata.add_sdparam("action", action);
  return sdata;
}

SData SData::new_printer(const QString& printer_name,
                         const QString& printer_path) {
  SData sdata("Printer@32473");
  sdata.add_sdparam("printerName", printer_name);
  sdata.add_sdparam("printerPath", printer_path);
  return sdata;
}

SData SData::new_printjob(const SubmitPrintRequest& request) {
  SData sdata("PrintJob@32473");
  sdata.add_sdparam("printer", request.printer);
  sdata.add_sdparam("filename", request.filename);
  sdata.add_sdparam("pageCount", QString(request.pageCount));
  sdata.add_sdparam("copies", QString(request.copies));
  return sdata;
}

SData SData::new_reservation(const QString& reservation_for) {
  SData sdata("Reservation@32473");
  sdata.add_sdparam("reservationFor", reservation_for);
  return sdata;
}

SData SData::new_serverreply(const QNetworkReply& reply) {
  SData sdata("PrintJobReply@32473");

  // If we had a network error, we can't have a status code so we fill it in
  // with the null value and report the network error
  QNetworkReply::NetworkError network_error = reply.error();
  sdata.add_sdparam("networkError", QString(network_error));
  if (network_error != QNetworkReply::NetworkError::NoError) {
    sdata.add_sdparam("statusCode", "-");
    sdata.add_sdparam("message", reply.errorString());
    return sdata;
  }

  // Since we got here, we should have an HTTP status code, but if that's not
  // OK then we should fill the message with the HTTP error code
  int status_code = reply.attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  sdata.add_sdparam("statusCode", QString(status_code));
  if (status_code != 200) {
    sdata.add_sdparam("message", reply.errorString());
    return sdata;
  }

  // No problems, no error message
  sdata.add_sdparam("message", "-");
  return sdata;
}

SData SData::new_settingvalue(const QString& key, const QString& value) {
  SData sdata("SettingValue@32473");
  sdata.add_sdparam("key", key);
  sdata.add_sdparam("value", value);
  return sdata;
}

SData SData::new_shell(const QString& shell) {
  SData sdata("Shell@32473");
  sdata.add_sdparam("shell", shell);
  return sdata;
}

SData SData::new_usermatch(const QString& user, const QString& stored_user,
                           bool allowed) {
  SData sdata("UserMatch@32473");
  sdata.add_sdparam("user", user);
  sdata.add_sdparam("storedUser", stored_user);
  sdata.add_sdparam("allowed", allowed ? "yes" : "no");
  return sdata;
}

SData SData::new_printerquery(const QString& printer) {
  SData sdata("PrinterQuery@32473");
  sdata.add_sdparam("printerName", printer);
  return sdata;
}

SData SData::new_printerresult(const PrintInfoReply& reply,
                               const QString& error) {
  SData sdata("PrinterResult@32473");
  sdata.add_sdparam("printerName", reply.printer);
  sdata.add_sdparam("currency", reply.currency);
  sdata.add_sdparam("costPerPage", QString::number(reply.costPerPage, 'f', 2));
  sdata.add_sdparam("funds", QString::number(reply.availableFunds, 'f', 2));
  sdata.add_sdparam("gratisBalance", QString::number(reply.availableGratis, 'f', 2));
  sdata.add_sdparam("gratisMethod", reply.gratisMethod);
  sdata.add_sdparam("error", error);
  return sdata;
}

SData SData::new_printerresult(const QString& error) {
  SData sdata("PrinterResult@32473");
  sdata.add_sdparam("printerName", "");
  sdata.add_sdparam("currency", "");
  sdata.add_sdparam("costPerPage", "");
  sdata.add_sdparam("funds", "");
  sdata.add_sdparam("gratisBalance", "");
  sdata.add_sdparam("gratisMethod", "");
  sdata.add_sdparam("error", error);
  return sdata;
}

void SData::add_sdparam(const QString& param_name, const QString& param_value) {
  this->m_sdata.insert(param_name, param_value);
}

QString SData::serialize_sdata() const {
  // RFC 5424 Section 3.1
  //  In case of zero structured data elements, the STRUCTURED-DATA field
  //  MUST contain the NILVALUE. ("-")
  if (m_sdata.empty()) {
    return QString("-");
  }

  QString sdata = QString("[%1").arg(m_sdid);

  for (QMap<QString, QString>::const_iterator i = m_sdata.constBegin();
    i != m_sdata.constEnd(); ++i) {
    sdata = QString("%1 %2=\"%3\"").arg(sdata, i.key(), i.value());
  }

  sdata.append(']');

  return sdata;
}


