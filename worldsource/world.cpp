#include "world.h"
#include <ctime>
#include <QImage>
#include <QRgb>
#include <QTimer>
#include <QDateTime>
#include <limits>
#include <iostream>
#include <QtDebug>

//class Tile
Tile::Tile(int xPosition, int yPosition, float tileWeight):
  xPos(xPosition), yPos(yPosition), value(tileWeight)
  {
  }

//class Enemy
Enemy::Enemy(int xPosition, int yPosition, float strength):
  Tile(xPosition, yPosition, strength), defeated{false}
  {
  }

//class Protagonist
Protagonist::Protagonist(): Tile(0, 0, 0.0f), health{100.0f}, energy{100.0f}
  {
  }

//class World
// methods throws a char * exception when file filename is not found
std::vector<std::unique_ptr<Tile>> World::createWorld(QString filename)
  {
  //clear used vector, correct usage expects first call to createWorld, then getEnemies followed by getHealthPacks
  used.clear();
  //read image in, every pixel greyvalue is value of tile, black pixels are considered as impassable, gets value infinity
  world.load(filename);
  if (world.isNull())
    throw "Unable to read image file " + filename;
  rows = world.height();
  cols = world.width();
  std::vector<std::unique_ptr<Tile>> tiles;
  tiles.reserve(rows*cols);
  for (int row = 0; row < rows; row++)
    {
    for (int col = 0; col < cols; col++)
      {
      float val = qGray(world.pixel(col,row))/255.0f;
      if (val > 0.0f)
        tiles.push_back(std::unique_ptr <Tile>(new Tile(col, row, val)));
      else
        tiles.push_back(std::unique_ptr<Tile>(new Tile(col, row, std::numeric_limits<float>::infinity())));
      }
    }
  return tiles;
  }

std::vector<std::unique_ptr<Enemy>> World::getEnemies(unsigned int nrOfEnemies)
  {
  qsrand(QDateTime::currentMSecsSinceEpoch());
  std::vector<std::unique_ptr<Enemy>> enemies;
  enemies.reserve(nrOfEnemies);
  int PEnemyCounter  = 0;
  while (enemies.size() < nrOfEnemies)
    {
    int xPos = qrand() % cols;
    int yPos = qrand() % rows;
    if (world.pixel(xPos, yPos) == 0xff000000)
      continue;
    std::unique_ptr<Enemy> eptr = nullptr;
    if ((xPos + yPos) % 4 != 0)
      {
      eptr = std::unique_ptr<Enemy>(new Enemy(xPos, yPos, qrand()%100));
      }
    else
      {
      eptr = std::unique_ptr<PEnemy>(new PEnemy(xPos, yPos, qrand()%100));
      PEnemyCounter++;
      }
    if (xPos != 0 || yPos != 0)
      {
      if (std::find_if(enemies.begin(), enemies.end(),  //lamba checks if there is already an enemy at the current position
                       [&eptr](std::unique_ptr<Enemy>& p)-> bool
      {return *p == *eptr;})
          == enemies.end())
        {
        enemies.push_back(std::move(eptr));
        used.push_back((QPoint(xPos, yPos)));
        }
      }
    }
  //be sure at least 1 PEnemy in the collection
  if (PEnemyCounter == 0)
    {
    auto & en = enemies.at(nrOfEnemies - 1);
    int xPos = en->getXPos();
    int yPos = en->getYPos();
    float val = en->getValue();
    enemies.pop_back();
    auto eptr = std::unique_ptr<PEnemy>(new PEnemy(xPos, yPos, val));
    enemies.push_back(std::move(eptr));
    }
  return enemies;
  }

std::vector<std::unique_ptr<Tile>> World::getHealthPacks(unsigned int nrOfPacks)
  {
  std::vector<std::unique_ptr<Tile>> healthPacks;
  healthPacks.reserve(nrOfPacks);
  while (healthPacks.size() < nrOfPacks)
    {
    int xPos = qrand() % cols;
    int yPos = qrand() % rows;
    if (world.pixel(xPos, yPos) == 0xff000000)
      continue;
    auto hptr = std::unique_ptr<Tile>(new Tile(xPos, yPos, qrand()%100));
    if (xPos != 0 || yPos != 0)
      {
      if (std::find_if(healthPacks.begin(), healthPacks.end(), //lamba checks if there is already a healthpack at the current position
                       [&hptr](std::unique_ptr<Tile>& p)-> bool
      {return *p == *hptr;})
          == healthPacks.end())
        {
        //check for enemy at same position
        if (std::find_if(used.begin(), used.end(),
                         [&xPos, &yPos](QPoint & p)-> bool
        {return (p.x() == xPos && p.y() == yPos) ;})
            == used.end())
          healthPacks.push_back(std::move(hptr));
        }
      }
    }
  return healthPacks;
  }

std::unique_ptr<Protagonist> World::getProtagonist()
  {
  return std::unique_ptr<Protagonist>(new Protagonist());
  }

//class PEnemy
PEnemy::PEnemy(int xPosition, int yPosition, float strength): Enemy(xPosition, yPosition, strength), poisonLevel{strength}
  {
  qsrand(time(nullptr));
  }

bool PEnemy::poison()
  {
  poisonLevel -= 10.0f;
  if (poisonLevel > 0.0f)
    {
    emit poisonLevelUpdated(poisonLevel);
    int t = qrand()%5;
    std::cout << "starting timer for " << t << " seconds" << " with poisonLevel = " << poisonLevel << std::endl;
    QTimer::singleShot(t*1000, this, SLOT(poison()));
    return true;
    }
  else
    {
    poisonLevel = 0.0f;
    emit dead();
    }
  return false;
  }

float PEnemy::getPoisonLevel() const
  {
  return poisonLevel;
  }

void PEnemy::setPoisonLevel(float value)
  {
  poisonLevel = value;
  }
