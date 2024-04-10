
#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include "glwidget.h"

#include <QObject>

class EventFilter : public QObject
{
public:
    EventFilter(QObject *parent = nullptr);
    bool eventFilter(QObject *watched, QEvent *event) override;

    GLWidget* target = nullptr;
    int goalHeight = 0;
    float aspectRatio = -1;
};

#endif // EVENTFILTER_H
