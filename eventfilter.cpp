
#include "eventfilter.h"
#include "glwidget.h"

#include <QEvent>
#include <QResizeEvent>
#include <QWidget>

EventFilter::EventFilter(QObject *parent)
    : QObject(parent) {}

bool EventFilter::eventFilter(QObject *watched, QEvent *event)
{
    // Intercept only target resize calls
    if(watched != target)
        return false;
    if(event->type() != QEvent::Resize)
        return false;
    qDebug() << "eventfilter call recieved";

    if (aspectRatio == -1)
    {
        qDebug() << "AR not set in event filter";
        return false;
    }

    QResizeEvent *resEvent = static_cast<QResizeEvent*>(event);
    goalHeight = 4 * resEvent->size().width() / 3;//aspectRatio;

    if(target->height()!=goalHeight)
        target->setHeight(goalHeight);

    return true;
}
