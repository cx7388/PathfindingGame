
#include <QObject>
#include <QGraphicsView>

#include "global_config.h"


class ZoomView : public QObject
{
    Q_OBJECT
public:
    ZoomView(QGraphicsView* view);

private:
    QGraphicsView* view;
    QPointF scene_pos;
    QPointF viewport_pos;
    bool eventFilter(QObject* object, QEvent* event);

signals:
    void zoomed();
};
