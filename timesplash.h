#ifndef TIMESPLASH
#define TIMESPLASH

#include <QObject>
#include <QSplashScreen>

class TimeSplash : public QSplashScreen
{
public:
    TimeSplash(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags());
    bool eventFilter(QObject *target, QEvent *event);
};

#endif // TIMESPLASH

