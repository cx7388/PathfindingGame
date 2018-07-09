#include "pixitem.h"

PixItem::PixItem():Tile(0, 0, 0.0f)
{

//    QPixmap pixPlayer;
//    pixPlayer.load(":map/pig.png");
//    QPixmap playerPixmap = pixPlayer.scaled(PIXEL_SIZE,PIXEL_SIZE, Qt::KeepAspectRatioByExpanding);
    //QGraphicsPixmapItem * player = new QGraphicsPixmapItem(playerPixmap);

    installEventFilter(this);

}
void PixItem::trymoveslot(int a, int b)
{
    setPos(a*SCALE_FACTOR,b*SCALE_FACTOR);

}


void PixItem::setasPlayer()
{
    setPixmap(QPixmap(":/map/pig.png").scaled(100,100));

}
void PixItem::setasHealthPack()
{
    setPixmap(QPixmap(":/map/healthPack.jpg").scaled(100,100));

}
void PixItem::setasEnemy()
{
    setPixmap(QPixmap(":/map/enemy.png").scaled(90,90));


}
void PixItem::setasPenemy()
{
    setPixmap(QPixmap(":/map/penemy.png").scaled(90,90));


}
void PixItem::setasDead()
{
    setPixmap(QPixmap(":/map/grave.jpg").scaled(100,100));


}


void PixItem::mousePressEvent(QMouseEvent *event)
{
    //pressLabel->setText(QString(tr("點擊處：(%1, %2)")).arg(QString::number(event->x()), QString::number(event->y())));

    int x = event->x();
    int y = event->y();
    qDebug()<<x<<y<<endl;

//    PixItem *curLabel = dynamic_cast<PixItem*>((event->pos()));
//    qDebug()<<curLabel->getYPos()<<endl;


//    QMimeData *data=new QMimeData;
//    data->setColor(Qt::green);
//    QDrag *drag=new QDrag(event->widget());
//    drag->setMimeData(data);
//    drag->start();
}

bool PixItem::eventFilter(QObject *object, QEvent *event)
{

    if(event->type()==QEvent::MouseButtonPress) {
        //  mousePressEvent((QMouseEvent*)event);

        mousePressEvent((QMouseEvent*)event);

        return true;

    } else if(event->type()==QEvent::DragEnter) {
        dragEnterEvent((QGraphicsSceneDragDropEvent * )event);
    } else {
        return QObject::eventFilter(object, event);
    }
}

void PixItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{

    qDebug()<<"drag enter"<<endl;
}

void PixItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{

    qDebug()<<"drag leave"<<endl;
}

void PixItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{

    qDebug()<<"drop"<<endl;
}

void PixItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    qDebug()<<"mouse move "<<endl;
}


