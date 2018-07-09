#include "mapcontroll.h"
#include "world.h"
#include "world_global.h"
#include "pixitem.h"
#include <QDebug>
#include <QList>
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <chrono>
#include <thread>
#include <QtCore>
#include <QTime>

MapControll::MapControll()
{

    world = std::make_shared<World>();
    pigger = new PixItem();
    pigger->setasPlayer();

    // readMap(mapPath);
    hero = world->getProtagonist();

    // rect = new Player(); // 暂时，我也不知道 到底用父类还是子类指

    //move X,Y
    connect(hero.get(), SIGNAL(posChanged(int,int )),pigger, SLOT(trymoveslot(int,int )));
    connect(hero.get(), SIGNAL(energyChanged(int)), this, SIGNAL(energy_value_change(int)));
    connect(hero.get(), SIGNAL(healthChanged(int)), this, SIGNAL(health_value_change(int)));

    //check collision
    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(handleCollisions()));
    timer->start(500);

//    timermove = new QTimer();
//    connect(timermove,SIGNAL(timeout()),rect,SLOT(movestep()));
//    timermove->start(100);




}


void MapControll::createEnemies(int num)
{
    enemies = world->getEnemies(num);
    for (auto  it = enemies.begin(); it != enemies.end(); it++) {
        if(this->isPenemy(it)) {
            std::shared_ptr<PEnemy> pen = std::make_shared<PEnemy>((*it)->getXPos(),(*it)->getYPos(),(*it)->getValue());
            enemyInfo.push_back(pen);
        } else {
            std::shared_ptr<Enemy> en = std::make_shared<Enemy>((*it)->getXPos(),(*it)->getYPos(),(*it)->getValue());
            enemyInfo.push_back(en);
        }

    }
    emit this->enmeyNumChanged(enemyInfo.size());
}

void MapControll::addHealthPackInfo()
{

    for (auto  it = healthPackInfo.begin(); it != healthPackInfo.end(); it++) {
        PixItem * healthitem = new PixItem();
        int x = (*it)->getXPos();
        int y = (*it)->getYPos();

        //每个加在地图上的health item 就会自带 value.
        healthitem->setValue((*it)->getValue());

        healthitem->setPos(SCALE_FACTOR*x,SCALE_FACTOR*y);
        // healthitem->setBrush(greenBrush);
        healthitem->setData(100,"health");
        healthitem->setasHealthPack();

        //drag and drop
        healthitem->setAcceptDrops(true);
        healthitem->setFlag(QGraphicsItem::ItemIsMovable,true);

        //||Not finish drag and drop because it can not rm from vector yet!


        mapscene->addItem(healthitem);

    }
}
void MapControll::addEnemyInfo()
{

    for (auto  it = enemyInfo.begin(); it != enemyInfo.end(); it++) {
        PixItem * enemyitem = new PixItem();
        int x = (*it)->getXPos();
        int y = (*it)->getYPos();

        enemyitem->setValue((*it)->getValue());//mapscene 上的item 有自带值

        // enemyitem->setRect(SCALE_FACTOR*x,SCALE_FACTOR*y,SCALE_FACTOR,SCALE_FACTOR);
        enemyitem->setPos(SCALE_FACTOR*x,SCALE_FACTOR*y);

        //qDebug() << "My name is " << x << ", nice to meet you!" << endl;

        if(this->isPenemy(it)) {
            //  enemyitem->setBrush(cyanBrush);
            enemyitem->setasPenemy();
            enemyitem->setData(200,"penemy");

            // enemyitem->boundingRect();
        } else {
            //enemyitem->setBrush(yellowBrush);
            enemyitem->setasEnemy();
            enemyitem->setData(300,"enemy");
            // enemyitem->boundingRect();


        }

        mapscene->addItem(enemyitem);
    }
}

void MapControll::setEnemies(std::vector<std::shared_ptr<Enemy>> enemyVector)
{

    for (auto  it = enemyVector.begin(); it != enemyVector.end(); it++) {
        if(this->isPenemy(it)) {
            std::shared_ptr<PEnemy> pen = std::make_shared<PEnemy>((*it)->getXPos(),(*it)->getYPos(),(*it)->getValue());
            enemyInfo.push_back(pen);
        } else {
            std::shared_ptr<Enemy> en = std::make_shared<Enemy>((*it)->getXPos(),(*it)->getYPos(),(*it)->getValue());
            enemyInfo.push_back(en);
        }

    }
    emit this->enmeyNumChanged(enemyInfo.size());
}

void MapControll::createHealthPack(int num)
{
    healthPacks = world->getHealthPacks(num);
    for (const auto& h : healthPacks) {

        //   int temp = h->getXPos();
        std::shared_ptr<Tile> he = std::make_shared<Tile>
                                   (h->getXPos(),h->getYPos(),h->getValue());
        healthPackInfo.push_back(he);


    }
    emit this->healthPackNumChanged(healthPackInfo.size());
}



void MapControll::setHealthPack(std::vector<std::shared_ptr<Tile>> healthPackSave)
{

    for (const auto& h : healthPackSave) {

        //   int temp = h->getXPos();
        std::shared_ptr<Tile> he = std::make_shared<Tile>
                                   (h->getXPos(),h->getYPos(),h->getValue());
        healthPackInfo.push_back(he);


    }
    emit this->healthPackNumChanged(healthPackInfo.size());
}

bool MapControll::move(QString str)
{
    //enter w will move up
    if(str == "w") {
        if(hero->getHealth()>0) {
            int y = hero->getYPos();
            if (y>0) {
                if(qIsFinite(worldInfo[(hero->getYPos())*world->getCols()+hero->getXPos()-world->getCols()]->getValue())) {
                    if(checkEnergy(hero->getXPos(),hero->getYPos(),hero->getXPos(),hero->getYPos()-1)) {
                        hero->setYPos(y-1);

                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    //enter a will move left
    if(str == "a") {
        if(hero->getHealth()>0) {
            int x = hero->getXPos();
            if (x>0) {
                if(qIsFinite(worldInfo[hero->getYPos()*world->getCols()+hero->getXPos()-1]->getValue())) {
                    if(checkEnergy(hero->getXPos(),hero->getYPos(),hero->getXPos()-1,hero->getYPos())) {
                        hero->setXPos(x-1);
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    //enter s will move up
    if(str == "s") {
        if(hero->getHealth()>0) {
            int y = hero->getYPos();
            if (y<world->getRows()) {
                if(qIsFinite(worldInfo[(hero->getYPos())*world->getCols()+hero->getXPos()+world->getCols()]->getValue())) {
                    if(checkEnergy(hero->getXPos(),hero->getYPos(),hero->getXPos(),hero->getYPos()+1)) {
                        hero->setYPos(y+1);
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    //enter d will move right
    if(str == "d") {
        if(hero->getHealth()>0) {
            int x = hero->getXPos();
            if (x<world->getCols()) {
                if(qIsFinite(worldInfo[hero->getYPos()*world->getCols()+hero->getXPos()+1]->getValue())) {
                    if(checkEnergy(hero->getXPos(),hero->getYPos(),hero->getXPos()+1,hero->getYPos())) {
                        hero->setXPos(x+1);
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        return false;
    } else {
        return false;
    }
}

std::vector<std::shared_ptr<Tile> > MapControll::getHealthPackInfo()
{
    return healthPackInfo;
}

std::vector<std::shared_ptr<Enemy> > MapControll::getenemyInfo()
{
    return enemyInfo;
}

std::shared_ptr<Tile> MapControll::selectEnemy()
{
    if(enemyInfo.size()!=0) {
        int distance = abs(enemyInfo[0]->getXPos()-hero->getXPos()) + abs(enemyInfo[0]->getYPos()-hero->getYPos());
        int count = 0;
        int index = 0;
        for(auto e:enemyInfo) {
            int d = abs(e->getXPos()-hero->getXPos()) + abs(e->getYPos()-hero->getYPos());
            if(d<distance) {
                index = count;
                distance = d;
            }
            count++;
        }
        return enemyInfo[index];
    } else {
        return NULL;
    }
}

std::shared_ptr<Tile> MapControll::selectHealthPack()
{
    if(healthPackInfo.size()!=0) {
        int distance = abs(healthPackInfo[0]->getXPos()-hero->getXPos()) + abs(healthPackInfo[0]->getYPos()-hero->getYPos());
        int count = 0;
        int index = 0;
        for(auto h:healthPackInfo) {
            int d = abs(h->getXPos()-hero->getXPos()) + abs(h->getYPos()-hero->getYPos());
            if(d<distance) {
                index = count;
                distance = d;
            }
            count++;
        }
        return healthPackInfo[index];
    } else {
        return NULL;
    }
}

QGraphicsScene *MapControll::getMapscene() const
{
    return mapscene;
}

PixItem* MapControll::getRect() const
{
    return pigger;
}

void MapControll::addEnemy()
{
//    QBrush yellowBrush(Qt::yellow);
//    QBrush cyanBrush(Qt::cyan);
    for (auto  it = enemies.begin(); it != enemies.end(); it++) {
        PixItem * enemyitem = new PixItem();
        int x = (*it)->getXPos();
        int y = (*it)->getYPos();

        enemyitem->setValue((*it)->getValue());//mapscene 上的item 有自带值

        // enemyitem->setRect(SCALE_FACTOR*x,SCALE_FACTOR*y,SCALE_FACTOR,SCALE_FACTOR);
        enemyitem->setPos(SCALE_FACTOR*x,SCALE_FACTOR*y);

        //qDebug() << "My name is " << x << ", nice to meet you!" << endl;

        if(this->isPenemy(it)) {
            //  enemyitem->setBrush(cyanBrush);
            enemyitem->setasPenemy();
            enemyitem->setData(200,"penemy");

            // enemyitem->boundingRect();
        } else {
            //enemyitem->setBrush(yellowBrush);
            enemyitem->setasEnemy();
            enemyitem->setData(300,"enemy");
            // enemyitem->boundingRect();


        }

        mapscene->addItem(enemyitem);
    }
}


void MapControll::addHealthPack()
{

    for (auto  it = healthPacks.begin(); it != healthPacks.end(); it++) {
        PixItem * healthitem = new PixItem();
        int x = (*it)->getXPos();
        int y = (*it)->getYPos();

        //每个加在地图上的health item 就会自带 value.
        healthitem->setValue((*it)->getValue());

        healthitem->setPos(SCALE_FACTOR*x,SCALE_FACTOR*y);
        // healthitem->setBrush(greenBrush);
        healthitem->setData(100,"health");
        healthitem->setasHealthPack();

        //drag and drop
        healthitem->setAcceptDrops(true);
        healthitem->setFlag(QGraphicsItem::ItemIsMovable,true);

        //||Not finish drag and drop because it can not rm from vector yet!


        mapscene->addItem(healthitem);

    }
}

void MapControll::rmEnemyfromScene()
{
    QList<QGraphicsItem *> items = mapscene->items();
    foreach (QGraphicsItem *item, items) {
        if(item->data(300)=="enemy") {
            mapscene->removeItem(item);
        }
        if(item->data(200)=="penemy") {
            mapscene->removeItem(item);
        }
    }

}

bool MapControll::isPenemy(auto e)
{
    PEnemy* isP = dynamic_cast<PEnemy*>(e->get());
    return isP;
}

void MapControll::readMap(QString mapPath)
{
    worldpixs = world->createWorld(mapPath); //create world
    worldInfo.clear();
    for (const auto& w : worldpixs) {
        std::shared_ptr<Tile> ws = std::make_shared<Tile>(w->getXPos(),w->getYPos(),w->getValue());
        worldInfo.push_back(ws);
    }
}




void MapControll::createMap()
{

    QBrush blackBrush(Qt::black);

    //item into scene
    for(auto &b :worldInfo) {
        QGraphicsRectItem * rectitem = new QGraphicsRectItem();
        rectitem->setRect(SCALE_FACTOR*b->getXPos(),SCALE_FACTOR*b->getYPos(),SCALE_FACTOR,SCALE_FACTOR);
        if(std::isinf(b->getValue()))
            rectitem->setBrush(blackBrush);
        else {
            rectitem->setBrush(QBrush(QColor(255*b->getValue(),255*b->getValue(),255*b->getValue(),255)));
        }
        mapscene->addItem(rectitem);
    }



    pigger->setFlag(QGraphicsItem::ItemIsFocusable);
    pigger->setFocus();

    mapscene->addItem(pigger);



}






void MapControll::handleCollisions()
{

    //如果被检测物的包含矩形（boundingRect()）完全包含在检测物内，算做碰撞；
    //  QList<QGraphicsItem *> collisions = pigger->collidingItems(Qt::ContainsItemShape);

    QList<QGraphicsItem *> collisions = pigger->collidingItems();


    foreach (QGraphicsItem *collidingItem, collisions) {
        //if health
        if (collidingItem->data(100) == "health") {

            //rm from scene
            mapscene->removeItem(collidingItem);

            //todo update health info
            PixItem* temp = dynamic_cast<PixItem*>(collidingItem);

            int temphealth = (hero->getHealth())+(temp->getValue());
            if(temphealth>100) temphealth = 100;
            hero->setHealth(temphealth);

            //rm from health vector = termimal also know( 黄河写了俩vector 得删俩)
            for (auto  it = healthPackInfo.begin(); it != healthPackInfo.end(); it++) {
                int xh = hero->getXPos();
                int yh = hero->getYPos();
                int x = (*it)->getXPos();
                int y = (*it)->getYPos();

                if(xh==x&&yh==y) {
                    healthPackInfo.erase(it);
                    emit this->healthPackNumChanged(healthPackInfo.size());
                    break;
                }
            }

            collisions.clear();
            //可以考虑发个信号， 让terminal 打印一下。
        }

        if (collidingItem->data(300) =="enemy") {

            mapscene->removeItem(collidingItem);


            //change to grave
            PixItem * grave = new PixItem();
            grave->setasDead();
            grave->setPos(SCALE_FACTOR*(hero->getXPos()),SCALE_FACTOR*(hero->getYPos()));
            mapscene->addItem(grave);


            PixItem* temp = dynamic_cast<PixItem*>(collidingItem);
            //energy and health
            hero->setEnergy(100);
            int temphealth = (hero->getHealth())-(temp->getValue());
            if(temphealth<0) temphealth = 0;
            hero->setHealth(temphealth);

            //rm from enemy vector
            for (auto  it = enemyInfo.begin(); it != enemyInfo.end(); it++) {
                int xh = hero->getXPos();
                int yh = hero->getYPos();
                int x = (*it)->getXPos();
                int y = (*it)->getYPos();

                if(xh==x&&yh==y) {
                    this->changeValue(x,y);
                    enemyInfo.erase(it);
                    emit this->enmeyNumChanged(enemyInfo.size());
                    break;

                }
                if(enemyInfo.empty()) {
                    break;
                }
            }
            collisions.clear();

            emit this->enemydefeated(); //signal for terminal print

        }



        if (collidingItem->data(200) =="penemy") {
            mapscene->removeItem(collidingItem);

            // change to grave
            PixItem * grave = new PixItem();
            grave->setasDead();
            grave->setPos(SCALE_FACTOR*(hero->getXPos()),SCALE_FACTOR*(hero->getYPos()));
            mapscene->addItem(grave);

            //health and energy
            PixItem* temp = dynamic_cast<PixItem*>(collidingItem);
            hero->setEnergy(100);

            int temphealth = (hero->getHealth())-(temp->getValue());
            if(temphealth<0) temphealth = 0;
            hero->setHealth(temphealth);

            //posion cloud
            posionclouditem = new QGraphicsEllipseItem();
            QBrush *brush = new QBrush();
            brush->setStyle(Qt::Dense5Pattern);
            brush->setColor(Qt::green);
            posionclouditem->setBrush(*brush);
            int xh = hero->getXPos();
            int yh = hero->getYPos();
            posionclouditem->setRect((xh-2)*SCALE_FACTOR,(yh-2)*SCALE_FACTOR, 10*SCALE_FACTOR, 10*SCALE_FACTOR);
            posionclouditem->setData(400,"posioncloud");
            mapscene->addItem(posionclouditem);

            for (auto  it = enemyInfo.begin(); it != enemyInfo.end(); ++it) {

                int xh = hero->getXPos();
                int yh = hero->getYPos();
                int x = (*it)->getXPos();
                int y = (*it)->getYPos();

                if(xh==x&&yh==y) {
                    this->changeValue(x,y); //change to infinite
                    PEnemy* penemydef = dynamic_cast<PEnemy*>(it->get());
                    connect(penemydef, SIGNAL(dead()),this,SLOT(penemydead()));
                    connect(penemydef,SIGNAL(poisonLevelUpdated(int)),this,SLOT(getposionUpdate(int)));
                    penemydef->poison();
                    break;


                }
                collisions.clear();
                if(enemyInfo.empty()) {
                    break;
                }

            }
            emit this->enemydefeated(); //signal for terminal print


        }

        if(collidingItem->data(400)=="posioncloud") {
            if(p_level!=old_plevel) {

                //    posionclouditem->setBrush(QBrush(QColor(255*p_level,255*p_level,255*p_level,255)));
                QBrush *brush = new QBrush();
                brush->setStyle(Qt::Dense5Pattern);
                brush->setColor(QColor(p_level*2.5,p_level*5,p_level*2.5,255));

                posionclouditem->setBrush(*brush);
                int temphealth = (hero->getHealth())-(p_level);
                if(temphealth<0) temphealth = 0;
                hero->setHealth(temphealth);
                old_plevel = p_level;
            }
            collisions.clear();

        }

    }


}

void MapControll::rmcircle(QGraphicsEllipseItem *posionitem)
{
    mapscene->addItem(posionitem);

}


void MapControll::setScaleH(int value)
{
    scaleH = value;
}

void MapControll::setScaleG(int value)
{
    scaleG = value;
}

void MapControll::penemydead()
{
    PEnemy* penemydef = dynamic_cast<PEnemy*>(sender());
    mapscene->removeItem(posionclouditem);


    for (auto  it = enemyInfo.begin(); it != enemyInfo.end(); ++it) {

        int xh = penemydef->getXPos();
        int yh = penemydef->getYPos();
        int x = (*it)->getXPos();
        int y = (*it)->getYPos();
        if(xh==x && yh==y) {
            enemyInfo.erase(it);
            emit this->enmeyNumChanged(enemyInfo.size());
            break;

        }
    }



}

QString MapControll::getMapPath() const
{
    return mapPath;
}

void MapControll::setMapPath(const QString &value)
{
    mapPath = value;
}

void  MapControll::setHeroValue(int x, int y, int health, float energy)
{
    hero->setXPos(x);
    hero->setYPos(y);
    hero->setHealth(health);
    hero->setEnergy(energy);

}


bool MapControll::autoMove(int goalx, int goaly)
{
    containHealthPack = false;
    Test test(hero->getXPos(),hero->getYPos(),goalx,goaly,scaleG,scaleH,world,worldInfo);
    qDebug()<<scaleG<<scaleH<<endl;
    //Test test(0,0,30,20,world,worldInfo);
    pathHero = test.getPathResult();
    for (auto it = pathHero.begin(); it!= pathHero.end()-1; it++) {
        if(healthPackInfo.size()!=0) {
            for (auto h = healthPackInfo.begin(); h!=healthPackInfo.end(); h++) {
                if ((*h)->getXPos()==(*it)->getXPos()&&(*h)->getYPos()==(*it)->getYPos()) {
                    containHealthPack = true;
                    break;
                }
            }
        }
    }
    return true;
}



bool MapControll::specificMove(int startx,int starty,int goalx, int goaly)
{
    Test test(startx,starty,goalx,goaly,scaleG,scaleH,world,worldInfo);
    qDebug()<<scaleG<<scaleH<<endl;
    hero->setXPos(startx);
    hero->setYPos(starty);
    pathHero = test.getPathResult();
    return true;
}

bool MapControll::movestep(int x, int y)
{
    if(checkEnergy(hero->getXPos(),hero->getYPos(),x,y)) {
        //std::cout<<abs(hero->getXPos()+hero->getYPos()-(*it)->getXPos()-(*it)->getYPos())<<std::endl;
        hero->setPos(x,y);

        if(!enemyInfo.empty()) {
            handleCollisions();
            emit this->enmeyNumChanged(enemyInfo.size());
        }
        //std::cout<<abs(hero->getXPos()+hero->getYPos()-(*it)->getXPos()-(*it)->getYPos())<<std::endl;
        return true;
    } else {
        return false;
    }
}

bool MapControll::checkEnergy(int x, int y, int cx, int cy)
{
    if(qIsFinite(worldInfo[y*world->getCols()+x]->getValue())) {
        //std::cout<<"not infinate"<<std::endl;
        float energyDiff = abs(worldInfo[y*world->getCols()+x]->getValue()-
                               worldInfo[cy*world->getCols()+cx]->getValue());
        if ((hero->getEnergy() - energyDiff - stepConsume)>0) {
            if(abs(x+y-cx-cy)==0) {
                return true;
            } else {
                hero->setEnergy(hero->getEnergy() - energyDiff - stepConsume);
                return true;
            }
        } else {
            return false;
        }
    } else {
        //std::cout<<"infinate"<<std::endl;
        if(hero->getEnergy()-stepConsume>0) {
            if(abs(x+y-cx-cy)==0) {
                return true;
            } else {
                hero->setEnergy(hero->getEnergy() - stepConsume);
                return true;
            }
        } else {
            return false;
        }


    }
}

std::shared_ptr<Tile> MapControll::getNearest()
{
    if(enemyInfo.size()>0) {
        int damage = 0;
        PEnemy* isP = dynamic_cast<PEnemy*>(this->selectEnemy().get());
        if(isP) {
            damage = this->selectEnemy()->getValue();
        } else {
            damage = this->selectEnemy()->getValue();
        }
        if(this->selectHealthPack()!=NULL) {
            int distance = abs((hero->getXPos())-this->selectHealthPack()->getXPos()) +
                           abs((hero->getYPos())-this->selectHealthPack()->getYPos());

            if(distance<distanceFactor&&hero->getHealth()<threshold) {
                if(containHealthPack == false) {
                    return this->selectHealthPack();
                } else {
                    if(hero->getHealth()-damage>0) {
                        return this->selectEnemy();
                    } else {
                        if(this->selectHealthPack()!=NULL) {
                            return this->selectHealthPack();
                        } else {
                            return NULL;
                        }
                    }
                }
            } else {
                if(hero->getHealth()-damage>0) {
                    return this->selectEnemy();
                } else {
                    if(this->selectHealthPack()!=NULL) {
                        return this->selectHealthPack();
                    } else {
                        return NULL;
                    }
                }
            }
        } else {
            if(hero->getHealth()-damage>0) {
                return this->selectEnemy();
            } else {
                if(this->selectHealthPack()!=NULL) {
                    return this->selectHealthPack();
                } else {
                    return NULL;
                }
            }
        }
    } else {
        return NULL;
    }
}

void MapControll::changeValue(int x, int y)
{
    //QBrush blackBrush(Qt::black);
    worldInfo[y*world->getCols()+x]->setValue(std::numeric_limits<float>::infinity());
//    QGraphicsRectItem * rectitem = new QGraphicsRectItem();
//    rectitem->setRect(SCALE_FACTOR*x,SCALE_FACTOR*y,SCALE_FACTOR,SCALE_FACTOR);
//    rectitem->setBrush(blackBrush);
//    mapscene->addItem(rectitem);
}

std::shared_ptr<Tile> MapControll::getPointValue(int x, int y)
{
    return worldInfo[y*world->getCols()+x];
}

void MapControll::changeEngrgy(int value)
{
    hero->setEnergy(value);
}

void MapControll::changeHealth(int value)
{
    hero->setHealth(value);
}

void MapControll::clearMap()
{
    worldInfo.clear();
}

void MapControll::clearEnemy()
{
//    for(auto  it = enemyInfo.begin(); it != enemyInfo.end(); it++) {
//        enemyInfo.erase(it);
//    }
    enemyInfo.clear();
    emit this->enmeyNumChanged(enemyInfo.size());
}

void MapControll::clearHealthPack()
{
//    for(auto  it = healthPackInfo.begin(); it != healthPackInfo.end(); it++) {
//        healthPackInfo.erase(it);
//    }
    healthPackInfo.clear();
    emit this->healthPackNumChanged(healthPackInfo.size());
}



void MapControll::getposionUpdate(int posionlevel)
{
    p_level = posionlevel;
    qDebug()<<p_level<<endl;
}


