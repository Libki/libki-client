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
* along with Libki. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMERWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QtGui>
#include <QtDebug>
#include <QMessageBox>
#include <QSettings>


#include "ui_timerwindow.h"

#include "networkclient.h"

class TimerWindow : public QMainWindow, public Ui::TimerWindow {

    Q_OBJECT

public:
    TimerWindow(QWidget *parent = 0);
    ~TimerWindow();
    void closeEvent(QCloseEvent *event);

signals:
    void requestLogout();
    void timerStopped();
    void serverAccountMinutesRequest();

public slots:
    void setAllowClose( bool );
    void startTimer( const QString& username, const QString& password, int minutes );
    void stopTimer();
    void updateTimeLeft( int minutes );
//    void showMessage( QString message );

private slots:
    void doLogoutDialog();
    void restoreTimerWindow();
    void iconActivated(QSystemTrayIcon::ActivationReason);
    void showSystemTrayIconTimeLeftMessage();

private:
    bool allowClose;

    QIcon libkiIcon;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QTimer* trayIconPopupTimer;
    int minutesRemaining;
    int minutesAtStart;

    void setupActions();

    void setupTrayIcon();

    void getSettings();

    void updateClock();
};

#endif // LOGINWINDOW_H
