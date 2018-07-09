#include "zoomview.h"
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>

ZoomView::ZoomView(QGraphicsView* view)
    : QObject(view), view(view)
{
    view->viewport()->installEventFilter(this);
    view->setMouseTracking(true);
}



bool ZoomView::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        viewport_pos = mouse_event->pos();
        scene_pos = view->mapToScene(mouse_event->pos());


    } else if (event->type() == QEvent::Wheel) {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);

        double angle = wheel_event->angleDelta().y();
        double factor = qPow(1.0015, angle);//x^y
        view->scale(factor, factor);
        view->centerOn(scene_pos);
        return true;
    } else {

        return QObject::eventFilter(object, event);
    }
}

