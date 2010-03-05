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

#include <QApplication>
#include <QFile>

#include "loginwindow.h"
#include "timerwindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  /* Apply the stylesheet */
  QFile qss("libki.qss");
  qss.open(QFile::ReadOnly);
  app.setStyleSheet(qss.readAll());
  qss.close();

  QCoreApplication::setOrganizationName("MillRunTech");
  QCoreApplication::setOrganizationDomain("MillRunTech.com");
  QCoreApplication::setApplicationName("Libki Kiosk Management System");

  LoginWindow* loginWindow = new LoginWindow();
  TimerWindow* timerWindow = new TimerWindow();

  QObject::connect( loginWindow, SIGNAL( loginSucceeded( const QString& , const QString& , int  ) ), 
           timerWindow, SLOT( startTimer( const QString& , const QString& , int  ) )
  );

  QObject::connect( timerWindow, SIGNAL( sessionEnded() ), loginWindow, SLOT( displayLoginWindow() ) );

  loginWindow->show();
                        
  return app.exec();
}
