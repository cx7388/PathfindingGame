#ifndef WORLD_H
#define WORLD_H

#include "world_global.h"
#include <vector>
#include <memory>
#include <QObject>
#include <QImage>

class Tile
{
public:
    Tile(int xPosition, int yPosition, float tileWeight);
    virtual ~Tile() =default;
    float getValue() const {return value;};
    void setValue(float newValue) {value = newValue;};
    int getXPos() const {return xPos;};
    int getYPos() const {return yPos;};
    void setXPos(int newPos) {xPos = newPos;};
    void setYPos(int newPos) {yPos = newPos;}
    bool operator== (const Tile & other) const
        {return (getXPos() == other.getXPos()) && (getYPos() == other.getYPos());};

protected:
    int xPos;
    int yPos;
    float value;
};

class Enemy : public QObject, public Tile
{
    Q_OBJECT
public:
    Enemy(int xPosition, int yPosition, float strength);
    virtual ~Enemy() = default;
    bool getDefeated() const {return defeated;}
    void setDefeated(bool value) {defeated = value; if (defeated) emit dead();};

  signals:
    void dead();

private:
    bool defeated; //false by construction
};

class PEnemy: public Enemy
{
    Q_OBJECT
public:
    PEnemy(int xPosition, int yPosition, float strength);
    virtual ~PEnemy() = default;
    float getPoisonLevel() const;
    void setPoisonLevel(float value);

public slots:
    bool poison();

signals:
    void poisonLevelUpdated(int value);

private:
    float poisonLevel;
};

class Protagonist: public QObject, public Tile
{
    Q_OBJECT
public:
    Protagonist();
    void setXPos(int newPos) {xPos = newPos; emit posChanged(xPos, yPos);}
    void setYPos(int newPos) {yPos = newPos; emit posChanged(xPos, yPos);}
    void setPos(int newX, int newY) {if (xPos != newX || yPos != newY) {xPos = newX; yPos = newY; emit posChanged(xPos, yPos);}} //判断不等于防止死循环
    float getHealth() const {return health;};
    void setHealth(float value) {health = value; emit healthChanged(static_cast<int>(health));}

    float getEnergy() const {return energy;}
    void setEnergy(float value) {energy = value; emit energyChanged(static_cast<int>(energy));}

signals:
    void posChanged(int x, int y);
    void healthChanged(int h);
    void energyChanged(int e);

private:
    float health; //100.0f by construction
    float energy; //100.0f by construction
};

class WORLDSHARED_EXPORT World
{
public:
    World() = default;
    //to obtain non-overlapping enemies and healthpacks you should call the following 3 methods in this order
    std::vector<std::unique_ptr<Tile>> createWorld(QString filename);
    std::vector<std::unique_ptr<Enemy>> getEnemies(unsigned int nrOfEnemies);
    std::vector<std::unique_ptr<Tile>> getHealthPacks(unsigned int nrOfPacks);
    std::unique_ptr<Protagonist> getProtagonist();
    int getRows() const {return rows;};
    int getCols() const {return cols;};

private:
    int rows, cols;
    QImage world;
    std::vector<QPoint> used; //only for internal use
};


#endif // WORLD_H
