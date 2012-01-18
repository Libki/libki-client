/*
* Copyright 2010 Kyle M Hall <kyle.m.hall@gmail.com>
*
* This file is part of Libki.
*
* Libki is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Libki is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Libki.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkclient.h"

NetworkClient::NetworkClient() : QObject() {
    qDebug("NetworkClient::NetworkClient");

    QSettings settings;

    nodeName = settings.value("node/name").toString();

    QString action = settings.value("node/logoutAction").toString();
    if ( action == "logout") {
        actionOnLogout = LogoutAction::Logout;
    } else if ( action == "reboot" ) {
        actionOnLogout = LogoutAction::Reboot;
    } else {
        actionOnLogout = LogoutAction::NoAction;
    }

    serviceURL.setHost( settings.value("server/host").toString() );
    serviceURL.setPort( settings.value("server/port").toInt() );
    serviceURL.setScheme( settings.value("server/scheme").toString() );

    registerNode();
    registerNodeTimer = new QTimer(this);
    connect(registerNodeTimer, SIGNAL(timeout()), this, SLOT(registerNode()) );
    registerNodeTimer->start( 1000 * 10 );

    updateUserDataTimer = new QTimer(this);
    connect(updateUserDataTimer, SIGNAL(timeout()), this, SLOT(getUserDataUpdate()));
}

void NetworkClient::attemptLogin( QString aUsername, QString aPassword ) {
    qDebug("NetworkClient::attemptLogin");

    username = aUsername;
    password = aPassword;

    serviceURL.addQueryItem( "username", username );
    serviceURL.addQueryItem( "password", password );

    QUrl loginURL = QUrl( serviceURL );
    loginURL.addQueryItem("action", "login");
    loginURL.addQueryItem("node", nodeName );

    qDebug() << "LOGIN URL: " << loginURL.toString();
    qDebug() << "NetworkClient::attemptLogin";

    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processAttemptLoginReply(QNetworkReply*)));

    QNetworkReply* reply = nam->get(QNetworkRequest(loginURL));
}

void NetworkClient::processAttemptLoginReply( QNetworkReply* reply ) {
    qDebug("NetworkClient::processAttemptLogoutReply");

    QByteArray result;
    result = reply->readAll();

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(result) + ")");

    if (sc.property("authenticated").toBoolean() == true){
        qDebug("Login Authenticated");

        int units = sc.property("units").toInteger();
        doLoginTasks( units );
    } else {
        qDebug("Login Failed");

        QString errorCode = sc.property("error").toString();
        qDebug() << "Error Code: " << errorCode;
        emit loginFailed( errorCode );
    }

    reply->deleteLater();

}

void NetworkClient::attemptLogout(){
    qDebug("NetworkClient::attemptLogout");

    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processAttemptLogoutReply(QNetworkReply*)));

    QUrl logoutURL = QUrl( serviceURL );
    logoutURL.addQueryItem("action", "logout");

    QNetworkReply* reply = nam->get(QNetworkRequest(logoutURL));
}

void NetworkClient::processAttemptLogoutReply( QNetworkReply* reply ) {
    qDebug("NetworkClient::processAttemptLogoutReply");

    QByteArray result;
    result = reply->readAll();

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(result) + ")");

    if (sc.property("logged_out").toBoolean() == true){
        doLogoutTasks();
    } else {
        emit logoutFailed();
    }

    reply->deleteLater();

}

void NetworkClient::getUserDataUpdate(){
    qDebug("NetworkClient::getUserDataUpdate");

    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processGetUserDataUpdateReply(QNetworkReply*)));

    QUrl userDataUrl = QUrl( serviceURL );
    userDataUrl.addQueryItem("action", "get_user_data");

    QNetworkReply* reply = nam->get(QNetworkRequest(userDataUrl));
}

void NetworkClient::processGetUserDataUpdateReply(QNetworkReply* reply) {
    qDebug("NetworkClient::processGetUserDataUpdateReply");

    QByteArray result;
    result = reply->readAll();

    qDebug() << "Server Result: " << result;

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(result) + ")");

    QString message = sc.property("message").toString();
    if ( !message.isEmpty() ) {
        this->clearMessage();
        emit messageRecieved( message );
    }

    QString status = sc.property("status").toString();
    if ( status == "Logged in" ) {
        int units = sc.property("units").toInteger();

        emit timeUpdatedFromServer( units );

        if ( units < 1 ) {
            doLogoutTasks();
        }

    } else if ( status == "Logged out" ) {
        doLogoutTasks();
    } else if ( status == "Kicked" ) {
        doLogoutTasks();
    }

    reply->deleteLater();
}

void NetworkClient::registerNode(){
    qDebug("NetworkClient::registerNode");

    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processRegisterNodeReply(QNetworkReply*)));

    QUrl registerUrl = QUrl( serviceURL );
    registerUrl.addQueryItem("action", "register_node");
    registerUrl.addQueryItem("node_name", nodeName );

    QNetworkReply* reply = nam->get(QNetworkRequest(registerUrl));
}

void NetworkClient::processRegisterNodeReply( QNetworkReply* reply ) {
    qDebug("NetworkClient::processRegisterNodeReply");

    QByteArray result;
    result = reply->readAll();

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(result) + ")");

    if (sc.property("registered").toBoolean() == true){
        //Really, there is nothing to do here.
    } else {
        qDebug("Node Registration FAILED");
    }

    reply->deleteLater();

}

void NetworkClient::clearMessage(){
    qDebug("NetworkClient::clearMessage");
    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processRegisterNodeReply(QNetworkReply*)));
    QUrl registerUrl = QUrl( serviceURL );
    registerUrl.addQueryItem("action", "clear_message");
    nam->get(QNetworkRequest(registerUrl));
}

void NetworkClient::processClearMessageReply( QNetworkReply* reply ){
    qDebug("NetworkClient::processClearMessageReply");
    reply->deleteLater();
}

void NetworkClient::doLoginTasks( int units ){

#ifdef Q_WS_WIN
    // If this is an MS Windows platform, use the keylocker programs to limit mischief.
    QProcess::startDetached("windows/on_login.exe");
#endif

    updateUserDataTimer->start( 1000 * 10 );
    emit loginSucceeded( username, password, units );
}

void NetworkClient::doLogoutTasks(){
    qDebug("NetworkClient::doLogoutTasks");
    updateUserDataTimer->stop();

    username.clear();
    password.clear();

#ifdef Q_WS_WIN
    //If this is an MS Windows platform, use the keylocker programs to limit mischief.
    QProcess::startDetached("windows/on_logout.exe");

    if ( actionOnLogout == LogoutAction::Logout ) {
        emit allowClose( true );
        QProcess::startDetached("shutdown -l -f");
    } else if ( actionOnLogout == LogoutAction::Reboot ) {
        emit allowClose( true );
        QProcess::startDetached("shutdown -r -f -t 0");
    }
#endif

    emit logoutSucceeded();
}
