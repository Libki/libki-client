#ifndef TIMESPLASH
#define TIMESPLASH

#include <QObject>
#include <QSplashScreen>

class TimerWindow;

class TimeSplash : public QSplashScreen
{
public:
    TimeSplash(TimerWindow*, const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags());
    bool eventFilter(QObject *target, QEvent *event);

private:
    TimerWindow* timerwindow;
};

#endif // TIMESPLASH

