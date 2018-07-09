#ifndef PIXPLAYER_H
#define PIXPLAYER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QDebug>
#include <QObject>
#include "world.h"
#include "global_config.h"

class PixItem :public QObject, public QGraphicsPixmapItem, public Tile
{

    Q_OBJECT

public:
    PixItem();

    void setasPlayer();
    void setasHealthPack();
    void setasEnemy();
    void setasPenemy();
    void setasDead();

    bool eventFilter(QObject *object, QEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);//处理drop
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);//添加QDrag

private slots:
    void trymoveslot(int a, int b);
};

#endif // PIXPLAYER_H
