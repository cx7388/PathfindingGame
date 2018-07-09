#ifndef MAPCONTROLL_H
#define MAPCONTROLL_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include <QApplication>
#include <QObject>
#include <QGraphicsEllipseItem>
#include <QKeyEvent>


#include <QString>
#include <QFile>
#include <string>
#include <vector>
#include <iostream>
#include <QtWidgets>
#include <QImage>
#include "world.h"
#include "world_global.h"
#include "global_config.h"
#include "pathfinder.h"
#include "ui_mainwindow.h"
#include "pixitem.h"


class MapControll :public QObject
{
    Q_OBJECT
public:
    MapControll();

    int getHeroXPos() const
    {
        return hero->getXPos();
    }

    int getHeroYPos() const
    {
        return hero->getYPos();
    }

    float getEnergy() const
    {
        return hero->getEnergy();
    }

    float getHealth() const
    {
        return hero->getHealth();
    }

    std::vector<Tile *> getPathHero() const
    {
        return pathHero;
    }

    std::vector<std::shared_ptr<Tile> > getWorldInfo() const
    {
        return worldInfo;
    }

    float getScaleH() const
    {
        return scaleH;
    }


    float getScaleG() const
    {
        return scaleG;
    }




    void createEnemies(int num);

    void setEnemies(std::vector<std::shared_ptr<Enemy>> enemyVector);

    void createHealthPack(int num);

    void setHealthPack(std::vector<std::shared_ptr<Tile>> healthPackSave);

    bool move(QString str);

    std::vector<std::shared_ptr<Tile>> getHealthPackInfo();

    std::vector<std::shared_ptr<Enemy>> getenemyInfo();

    void createMap();

    std::shared_ptr<Tile> selectEnemy();

    std::shared_ptr<Tile> selectHealthPack();

    QGraphicsScene *getMapscene() const;

    PixItem* getRect() const;

    void moveMap();

    void addEnemy();

    void addHealthPack();

    bool isPenemy(auto e);

    bool autoMove(int goalx,int goaly);

    bool specificMove(int startx,int starty,int goalx, int goaly);

    bool movestep(int x, int y);

    bool checkEnergy(int x,int y, int cx, int cy);

    std::shared_ptr<Tile> getNearest();

    void changeValue(int x, int y);

    std::shared_ptr<Tile> getPointValue(int x, int y);


    void changeEngrgy(int value);
    void changeHealth(int value);


    void readMap(QString mapPath);

    void clearMap();

    void clearEnemy();

    void clearHealthPack();

    void rmEnemyfromScene();

    void addEnemyInfo();

    void addHealthPackInfo();

    void setMapPath(const QString &value);

    QString getMapPath() const;

    void setHeroValue(int x,int y,int health, float energy);

signals:
    void energy_value_change(int );
    void health_value_change(int );
    void enmeyNumChanged(int );
    void healthPackNumChanged(int );
    void enemydefeated();
    void startPoison();

private slots:

    void getposionUpdate(int posionlevel);
    void handleCollisions();
    void rmcircle(QGraphicsEllipseItem * posionclouditem );

    void setScaleH(int value);


    void setScaleG(int value);
    void penemydead();


private:
    QTimer * timer ;
    QTimer * timermove ;

    int p_level = 0;
    std::shared_ptr<World> world;

    std::vector<std::shared_ptr<Tile>> healthPackInfo;
    std::vector<std::shared_ptr<Enemy>> enemyInfo;
    std::vector<std::shared_ptr<Tile>> worldInfo;

    std::vector<std::unique_ptr<Tile>> worldpixs;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Tile>> healthPacks;

    //myy create map
    std::unique_ptr<Protagonist> hero;  //player   这里是子类指向子类
    //Protagonist * hero;
    std::vector<std::unique_ptr<Tile>> mTiles;
    QGraphicsScene * mapscene = new QGraphicsScene;

    // Player * rect;
    PixItem* pigger;

    std::vector<Tile*> pathHero;

    float factor = 1.2;

    float stepConsume = 0.1;

    int old_plevel= 9999;
    float scaleG = 1;
    float scaleH = 1;

    int distanceFactor = 5;

    int threshold = 70;

    bool containHealthPack =false;

    QString mapPath =":/map/worldmap4.png";
    QGraphicsEllipseItem * posionclouditem ;


};

#endif // MAPCONTROLL_H
