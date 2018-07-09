#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <QDialog>
#include <QDebug>
#include <QSettings>

#include "world.h"
#include "world_global.h"
#include "global_config.h"
#include "mapcontroll.h"
#include "zoomview.h"
#include "mapcontroll.h"
#include "ui_mainwindow.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    QGraphicsScene * mapscene;

    void initMap();

    ~MainWindow();


    Ui::MainWindow *getUi() const;

    void keyPressEvent(QKeyEvent * event);
    bool eventFilter(QObject *object, QEvent *event);
    void mousePressEvent(QMouseEvent *event);


    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);//处理drop
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);//添加QDrag



    bool getPause() const
    {
        return pause;
    }

    void resetgame(QString mappath);

public slots:
    void movestep_slot();

    void specfic_step_slot();

    void setMoveSpeed(int speed);


private slots:

    void on_pushButton_clicked(); //init button

    void on_command_returnPressed();

    void on_EnemyInput_returnPressed();

    void on_HealthPackInput_returnPressed();

    void getStatus();

    void getEnemiesInfo();

    void getHealthPacksInfo();

    void getNearestEnemy();

    void getNearestHealthPack();

    void getNearestthing();

    void getMapInfo(int x, int y);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();


    void victroy();

    void gameOver();



    void on_btn_goto_clicked();

    void on_btn_magic_clicked();



    void on_pause_clicked();

    void on_save_clicked();

    void on_load_clicked();

    void enemydefectslot();

private:
    Ui::MainWindow *ui;

    std::shared_ptr<MapControll> controll;

    std::vector<Tile*> results;

    void move_next();

    int moveSpeed = 1;

    std::vector<QGraphicsRectItem *> pathItem;

    std::shared_ptr<Tile> destination;

    bool pause = false;

    void SaveSettings();
    void LoadSettings();

    bool firstGame = true;

    bool specific = false;
};

#endif // MAINWINDOW_H
