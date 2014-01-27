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

#include <QtNetwork/QHostInfo>

NetworkClient::NetworkClient() : QObject() {
    qDebug("NetworkClient::NetworkClient");

    QSettings settings;

    nodeName = settings.value("node/name").toString();

    // Fail over to hostname if node name isn't defined.
    if ( nodeName.isEmpty() ) {
        QHostInfo hostInfo;
        hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
        nodeName = QHostInfo::localHostName();
    }

    nodeLocation = settings.value("node/location").toString();
//    settings.dumpObjectInfo();
//    settings.dumpObjectTree();
//    qDebug() << "QSettings Path: " << settings.fileName();

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
    serviceURL.setPath("/api/client/v1_0");

	urlQuery.addQueryItem("node", nodeName );
    urlQuery.addQueryItem("location", nodeLocation );

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

    QUrl url = QUrl( serviceURL );
	QUrlQuery query = QUrlQuery(urlQuery);
    query.addQueryItem("action", "login");
    query.addQueryItem( "username", username );
    query.addQueryItem( "password", password );
	url.setQuery(query);

    qDebug() << "LOGIN URL: " << url.toString();
    qDebug() << "NetworkClient::attemptLogin";

    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processAttemptLoginReply(QNetworkReply*)));

    /*QNetworkReply* reply = */nam->get(QNetworkRequest(url));
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

        username.clear();
        password.clear();

        emit loginFailed( errorCode );
    }

    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();
}

void NetworkClient::attemptLogout(){
    qDebug("NetworkClient::attemptLogout");

    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processAttemptLogoutReply(QNetworkReply*)));

    QUrl url = QUrl( serviceURL );
	QUrlQuery query = QUrlQuery(urlQuery);
    query.addQueryItem("action", "logout");
    query.addQueryItem( "username", username );
    query.addQueryItem( "password", password );
	url.setQuery(query);
    /*QNetworkReply* reply =*/ nam->get(QNetworkRequest(url));
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

    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();
}

void NetworkClient::getUserDataUpdate(){
    qDebug("NetworkClient::getUserDataUpdate");

    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processGetUserDataUpdateReply(QNetworkReply*)));

    QUrl url = QUrl( serviceURL );
	QUrlQuery query = QUrlQuery(urlQuery);
    query.addQueryItem("action", "get_user_data");
    query.addQueryItem( "username", username );
    query.addQueryItem( "password", password );
	url.setQuery(query);
    /*QNetworkReply* reply =*/ nam->get(QNetworkRequest(url));
}

void NetworkClient::processGetUserDataUpdateReply(QNetworkReply* reply) {
    qDebug("NetworkClient::processGetUserDataUpdateReply");

    QByteArray result;
    result = reply->readAll();

    qDebug() << "Server Result: " << result;

    QScriptValue sc;
    QScriptEngine engine;
    QString json = "(" + QString(result) + ")";

    if ( engine.canEvaluate(json) ){
        sc = engine.evaluate( json );

        if ( ! engine.hasUncaughtException() ) {

//            QString message = sc.property("message").toString();
//            if ( !message.isEmpty() && !message.isNull() ) {
//                this->clearMessage();
//                emit messageRecieved( message );
//            }

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
        }
    }

    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();
}

void NetworkClient::registerNode(){
    qDebug("NetworkClient::registerNode");

    QNetworkAccessManager* nam;
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(processRegisterNodeReply(QNetworkReply*)));

    QUrl url = QUrl( serviceURL );
	QUrlQuery query = QUrlQuery(urlQuery);
    query.addQueryItem("action", "register_node");
    query.addQueryItem("node_name", nodeName );
	url.setQuery(query);

    /*QNetworkReply* reply =*/ nam->get(QNetworkRequest(url));
}

void NetworkClient::processRegisterNodeReply( QNetworkReply* reply ) {
    qDebug("NetworkClient::processRegisterNodeReply");

    QByteArray result;
    result = reply->readAll();

    qDebug() << "Server Result: " << result;

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(result) + ")");

    if ( !sc.property("registered").toBoolean() ){
        qDebug("Node Registration FAILED");
    }

    QSettings settings;

    QString bannerTopURL = settings.value("session/BannerTopURL").toString();
    QString bannerBottomURL = settings.value("session/BannerBottomURL").toString();

    settings.setValue("session/ClientBehavior", sc.property("ClientBehavior").toString());
    settings.setValue("session/ReservationShowUsername", sc.property("ReservationShowUsername").toString());

    settings.setValue("session/BannerTopURL", sc.property("BannerTopURL").toString());
    settings.setValue("session/BannerTopWidth", sc.property("BannerTopWidth").toString());
    settings.setValue("session/BannerTopHeight", sc.property("BannerTopHeight").toString());

    settings.setValue("session/BannerBottomURL", sc.property("BannerBottomURL").toString());
    settings.setValue("session/BannerBottomWidth", sc.property("BannerBottomWidth").toString());
    settings.setValue("session/BannerBottomHeight", sc.property("BannerBottomHeight").toString());

    settings.sync();

    if ( bannerTopURL != sc.property("BannerTopURL").toString() || bannerBottomURL != sc.property("BannerBottomURL").toString() ) {
        emit handleBanners(); //TODO: Emit only if a banner url has changed
    }

    QString reserved_for = sc.property("reserved_for").toString();
    emit setReservationStatus( reserved_for );

    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();

}

void NetworkClient::clearMessage(){
    qDebug("NetworkClient::clearMessage");
    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(ignoreNetworkReply(QNetworkReply*)));
    QUrl url = QUrl( serviceURL );
    QUrlQuery query = QUrlQuery(urlQuery);
    query.addQueryItem("action", "clear_message");
    query.addQueryItem( "username", username );
    query.addQueryItem( "password", password );
	url.setQuery(query);
    nam->get(QNetworkRequest(url));
}

void NetworkClient::acknowledgeReservation( QString reserved_for ){
    qDebug("NetworkClient::acknowledgeReservation");
    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(ignoreNetworkReply(QNetworkReply*)));
    QUrl url = QUrl( serviceURL );
	QUrlQuery query = QUrlQuery(urlQuery);
    query.addQueryItem("action", "acknowledge_reservation");
    query.addQueryItem("reserved_for", reserved_for);
	url.setQuery(query);

    nam->get(QNetworkRequest(url));
}

void NetworkClient::ignoreNetworkReply( QNetworkReply* reply ){
    qDebug("NetworkClient::ignoreNetworkReply");

    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();
}

void NetworkClient::doLoginTasks( int units ){

#ifdef Q_OS_WIN
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

#ifdef Q_OS_WIN
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

#ifdef Q_OS_UNIX
    if ( actionOnLogout == LogoutAction::Logout ) {
        emit allowClose( true );
        // Restart KDE 4
        QProcess::startDetached("qdbus org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout -0 -1 -1");
        // Restart Gnome
        QProcess::startDetached("gnome-session-save --kill --silent");
        // Restart XFCE 4
        QProcess::startDetached("/usr/bin/xfce4-session-logout");
        // Restart Mate
        QProcess::startDetached("mate-session-save --force-logout");
    } else if ( actionOnLogout == LogoutAction::Reboot ) {
        emit allowClose( true );
        // For this to work, sudo must be installed and the line
        // %shutdown ALL=(root) NOPASSWD: /sbin/reboot
        // needs to be added to /etc/sudoers
        QProcess::startDetached("sudo reboot");
    }
#endif

    emit logoutSucceeded();
}
