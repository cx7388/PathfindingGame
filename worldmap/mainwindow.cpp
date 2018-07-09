#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pathfinder.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //make it scrollable
    ZoomView* z = new ZoomView(ui->graphicsView);


    //change the background
    ui->graphicsView->setBackgroundBrush(Qt::gray);


    controll = std::make_shared<MapControll>();

    destination = std::make_shared<Tile>(0,0,0);

    ui->graphicsView->show();
    ui->terminal->hide();

    //SIGNAL SLOT
    //QObject::connect(controll->getRect(), SIGNAL(energyChanged(int)), ui->energybar, SLOT(setValue(int)));

    QObject::connect(controll.get(), SIGNAL(energy_value_change(int)), ui->energybar, SLOT(setValue(int)));
    QObject::connect(controll.get(), SIGNAL(health_value_change(int)), ui->healthbar, SLOT(setValue(int)));

    //show the number of enemy and health pack
    QObject::connect(controll.get(),SIGNAL(enmeyNumChanged(int)),ui->enemyNum,SLOT(display(int)));
    QObject::connect(controll.get(),SIGNAL(healthPackNumChanged(int)),ui->healthPackNum,SLOT(display(int)));
    //connet set speed two UI
    QObject::connect(ui->speed_box, SIGNAL(valueChanged(int)), ui->speed_slider_bar, SLOT(setValue(int)));
    QObject::connect(ui->speed_slider_bar, SIGNAL(valueChanged(int)), ui->speed_box, SLOT(setValue(int)));
    QObject::connect(ui->speed_box, SIGNAL(valueChanged(int)), this, SLOT(setMoveSpeed(int)));

    //connect A*(G, H)
    QObject::connect(ui->g_box, SIGNAL(valueChanged(int)), ui->g_slider, SLOT(setValue(int)));
    QObject::connect(ui->g_slider, SIGNAL(valueChanged(int)), ui->g_box, SLOT(setValue(int)));
    QObject::connect(ui->h_box, SIGNAL(valueChanged(int)), ui->h_slider, SLOT(setValue(int)));
    QObject::connect(ui->h_slider, SIGNAL(valueChanged(int)), ui->h_box, SLOT(setValue(int)));


    QObject::connect(ui->h_slider, SIGNAL(valueChanged(int)), controll.get(), SLOT(setScaleH(int)));
    QObject::connect(ui->g_slider, SIGNAL(valueChanged(int)), controll.get(), SLOT(setScaleG(int)));


    //connect handleconlision with terminal
    QObject::connect(controll.get(),SIGNAL(enemydefeated()),this,SLOT(enemydefectslot()));

    mapscene = controll->getMapscene();

    ui->graphicsView->setScene(mapscene);
    ui->graphicsView->scale(0.1,0.1);
    ui->graphicsView->show();
}

// init create map add player.
void MainWindow::on_pushButton_clicked()
{
    if(firstGame == true) {
        controll->readMap(controll->getMapPath());
        controll->createMap();

        ui->terminal->append("World created, hero reborn");
        this->getStatus();
        firstGame = false;
    } else {
        resetgame(controll->getMapPath());
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

//call conroller to run it.
//conytoller->lllll
void MainWindow::on_command_returnPressed()
{
    ui->graphicsView->centerOn(controll->getRect());
    QString str = ui->command->text();
    ui->command->clear();
    ui->terminal->append(str);
    if (str == NULL) {
        return;
    }

    //enter clear will clean the terminal
    if (str == "clear") {
        ui->terminal->clear();
        return;
    }

    if (str == "status") {
        this->getStatus();
    }

    //enter w will move up
    if(str == "w") {
        if(controll->getHealth()>0&&controll->getEnergy()>0) {
            if (controll->move(str)) {
                ui->terminal->append("Move up 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else {
            ui->terminal->append("You are dead!");
            this->gameOver();
        }
    }

    //enter a will move left
    if(str == "a") {
        if(controll->getHealth()>0&&controll->getEnergy()>0) {
            if (controll->move(str)) {
                ui->terminal->append("Move left 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else {
            ui->terminal->append("You are dead!");
            this->gameOver();
        }
    }

    //enter s will move down
    if(str == "s") {
        if(controll->getHealth()>0&&controll->getEnergy()>0) {
            if (controll->move(str)) {
                ui->terminal->append("Move down 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else {
            ui->terminal->append("You are dead!");
            this->gameOver();
        }
    }

    //enter d will move right
    if(str == "d") {
        if(controll->getHealth()>0&&controll->getEnergy()>0) {
            if (controll->move(str)) {
                ui->terminal->append("Move right 1 step");
                // controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else {
            ui->terminal->append("You are dead!");
            this->gameOver();
        }
    }

    //enter enemy will get info of enemies
    if(str == "enemy") {
        this->getEnemiesInfo();
    }

    //enter healthpack will get info of health packs
    if(str == "healthpack") {
        this->getHealthPacksInfo();
    }

    //enter selectEnemy to choose nearest enmey
    if(str == "selectEnemy") {
        this->getNearestEnemy();
    }

    //enter selectHealthPack to choose nearest health pack
    if(str == "selectHealthPack") {
        this->getNearestHealthPack();
    }

    //enter get to choose nearest thing
    if(str == "get") {
        this->getNearestthing();
    }

    if(str == "point") {
        this->getMapInfo(controll->getHeroXPos(),controll->getHeroYPos());
    }

    if(str == "changemap") {
        QString fileName;
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open Image"), "/home", tr("Image Files (*.png *.jpg *.bmp)"));
        if(fileName !=NULL)
            resetgame(fileName);
    }
}

void MainWindow::on_EnemyInput_returnPressed()
{
    QString qstr = ui->EnemyInput->text();
    std::string str = qstr.toUtf8().constData();
    int number = std::stoi(str);
    controll->createEnemies(number); //create enemies
    //  controll->rmEnemyfromScene();//rm then add otherwise will be overlap
    //but it will make it slow....
    controll->addEnemy();
    ui->terminal->append("Generate "+QString::number(number)+" Enemies");
    ui->terminal->append(" ");
}

void MainWindow::on_HealthPackInput_returnPressed()
{
    QString qstr = ui->HealthPackInput->text();
    std::string str = qstr.toUtf8().constData();
    int number = std::stoi(str);
    controll->createHealthPack(number); //createHealthPcaks

    /**Add  health pack  */
    controll->addHealthPack();

    ui->terminal->append("Generate "+QString::number(number)+" Health Pcaks");
    ui->terminal->append(" ");
}

void MainWindow::getStatus()
{
    ui->terminal->append("X position: "+QString::number(controll->getHeroXPos()));
    ui->terminal->append("Y position: "+QString::number(controll->getHeroYPos()));
    ui->terminal->append("Energy: "+QString::number(controll->getEnergy()));
    ui->terminal->append("Health: "+QString::number(controll->getHealth()));
    ui->terminal->append(" ");

//    //for bar
//    ui->energybar->setValue(controll->getEnergy());
//    ui->healthbar->setValue(controll->getHealth());
}

void MainWindow::getEnemiesInfo()
{
    int i = 1;
    for(auto e:controll->getenemyInfo()) {
        ui->terminal->append("The " + QString::number(i) + " Enemy");
        ui->terminal->append("X position: "+QString::number(e->getXPos()));
        ui->terminal->append("Y position: "+QString::number(e->getYPos()));

        PEnemy* isP = dynamic_cast<PEnemy*>(e.get());
        if(isP) {
            ui->terminal->append("Posion Enemy");
            ui->terminal->append("Posion Value: "+QString::number(e->getValue()));
        } else {
            ui->terminal->append("Normal Enemy");
            ui->terminal->append("Strength: "+QString::number(e->getValue()));
        }
        ui->terminal->append(" ");

        i++;
    }
}

void MainWindow::getHealthPacksInfo()
{
    int i = 1;
    for(auto &h:controll->getHealthPackInfo()) {
        ui->terminal->append("The " + QString::number(i) + " Health Packs");
        ui->terminal->append("X position: "+QString::number(h->getXPos()));
        ui->terminal->append("Y position: "+QString::number(h->getYPos()));
        ui->terminal->append("Value: "+QString::number(h->getValue()));
        ui->terminal->append(" ");
        i++;
    }
}

void MainWindow::getNearestEnemy()
{
    if(controll->selectEnemy()!=NULL) {
        ui->terminal->append("Nearest enemy found at position:");
        ui->terminal->append("X position: "+QString::number(controll->selectEnemy()->getXPos()));
        ui->terminal->append("Y position: "+QString::number(controll->selectEnemy()->getYPos()));
        PEnemy* isP = dynamic_cast<PEnemy*>(controll->selectEnemy().get());
        if(isP) {
            ui->terminal->append("Posion Enemy");
            ui->terminal->append("Posion Value: "+QString::number(controll->selectEnemy()->getValue()));
        } else {
            ui->terminal->append("Normal Enemy");
            ui->terminal->append("Strength: "+QString::number(controll->selectEnemy()->getValue()));
        }
        ui->terminal->append(" ");
    } else {
        ui->terminal->append("No Enemy!");
        ui->terminal->append(" ");
    }
    ui->terminal->append(" ");
}

void MainWindow::getNearestHealthPack()
{
    if(controll->selectHealthPack()!=NULL) {
        ui->terminal->append("Nearest health pack found at position:");
        ui->terminal->append("X position: "+QString::number(controll->selectHealthPack()->getXPos()));
        ui->terminal->append("Y position: "+QString::number(controll->selectHealthPack()->getYPos()));
        ui->terminal->append("Value: "+QString::number(controll->selectHealthPack()->getValue()));
        ui->terminal->append(" ");
    } else {
        ui->terminal->append("No HealthPack!");
        ui->terminal->append(" ");
    }
}

void MainWindow::getNearestthing()
{
    if(controll->getNearest()!=NULL) {
        Enemy* isE = dynamic_cast<Enemy*>(controll->getNearest().get());
        if (isE) {
            this->getNearestEnemy();
        } else {
            this->getNearestHealthPack();
        }
    } else {
        this->getNearestEnemy();
        this->getNearestHealthPack();
    }
}

void MainWindow::getMapInfo(int x, int y)
{
    ui->terminal->append("Now point info:");
    ui->terminal->append("X position: "+QString::number(controll->getPointValue(x,y)->getXPos()));
    ui->terminal->append("Y position: "+QString::number(controll->getPointValue(x,y)->getYPos()));
    ui->terminal->append("Value: "+QString::number(controll->getPointValue(x,y)->getValue()));
}


//void display_vector(vector<unique_ptr<int>> vec)
//{
//    for (auto it = vec.begin(); it != vec.end(); it++)
//    {
//         << **it << endl;
//    }
//}



//计算路径然后打印
void MainWindow::on_pushButton_2_clicked()
{
    specific = false;
    pause =false;
    int index = 0;
    for(auto a=pathItem.begin(); a<pathItem.end(); a++) {
        mapscene->removeItem(pathItem[index]);
        index ++;
    }
    pathItem.clear();
    results.clear();
    if(controll->getNearest()!=NULL) {
        destination = controll->getNearest();
        int xgoal = controll->getNearest()->getXPos();
        int ygoal = controll->getNearest()->getYPos();
        this->getNearestthing();
        //!!!!!!change it!!!!!!!!!!!!!!
        controll->autoMove(xgoal,ygoal);
        results = controll->getPathHero();

        int count = 0;
        for(auto a=results.begin(); a<results.end()-1; a++) {
            //   QBrush blueBrush(Qt::blue);
            QGraphicsRectItem * path = new QGraphicsRectItem;
            path->setRect((results[count]->getXPos())*SCALE_FACTOR,(results[count]->getYPos())*SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
            path->setBrush(Qt::blue);
            mapscene->addItem(path);
            pathItem.push_back(path);
            count ++;
        }

        movestep_slot();
    } else {
        if(controll->getenemyInfo().size()==0) {
            this->getNearestEnemy();
            this->getNearestHealthPack();
            std::string str = "Caution";
            QString qstr=QString::fromStdString(str);
            QMessageBox::information(this,qstr,tr("You need enemy!"));

        } else {
            this->getNearestEnemy();
            this->getNearestHealthPack();
            this->gameOver();

        }
    }

}

void MainWindow::movestep_slot()
{

    if(specific == true) {

    } else {
        if(getPause() == false&&results.size()!=0) {

            auto it = results.begin();

            if(controll->movestep((*it)->getXPos(),(*it)->getYPos())) {
                ui->graphicsView->centerOn(controll->getRect());
                this->getStatus();

                if(pathItem.size()!=0) {
                    mapscene->removeItem(pathItem[0]);
                    pathItem.erase(pathItem.begin());
                }

                //erase and call timer
                if(results.size()!=0) {
                    results.erase(results.begin());
                }
                if(results.size()!=0) {

                    if(destination->getXPos()==controll->getNearest()->getXPos()&&destination->getYPos()==controll->getNearest()->getYPos()) {
                        QTimer::singleShot(moveSpeed, this, SLOT(movestep_slot()));
                    } else {
                        destination = controll->getNearest();
                        int xgoal = controll->getNearest()->getXPos();
                        int ygoal = controll->getNearest()->getYPos();
                        this->getNearestthing();


                        int index = 0;
                        for(auto a=pathItem.begin(); a<pathItem.end(); a++) {
                            mapscene->removeItem(pathItem[index]);
                            index ++;
                        }
                        pathItem.clear();
                        results.clear();

                        controll->autoMove(xgoal,ygoal);
                        results = controll->getPathHero();

                        int count = 0;
                        for(auto a=results.begin(); a<results.end()-1; a++) {
                            //   QBrush blueBrush(Qt::blue);
                            QGraphicsRectItem * path = new QGraphicsRectItem;
                            path->setRect((results[count]->getXPos())*SCALE_FACTOR,(results[count]->getYPos())*SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
                            path->setBrush(Qt::blue);
                            mapscene->addItem(path);
                            pathItem.push_back(path);
                            count ++;
                        }
                        QTimer::singleShot(moveSpeed, this, SLOT(movestep_slot()));
                    }
                }
                if(results.size()==0) {
                    if(controll->getNearest()!=NULL) {
                        destination = controll->getNearest();
                        int xgoal = controll->getNearest()->getXPos();
                        int ygoal = controll->getNearest()->getYPos();
                        this->getNearestthing();
                        controll->autoMove(xgoal,ygoal);
                        results = controll->getPathHero();
                        int index1 = 0;
                        for(auto a=pathItem.begin(); a<pathItem.end(); a++) {
                            mapscene->removeItem(pathItem[index1]);
                            index1 ++;
                        }
                        int count = 0;
                        for(auto a=results.begin(); a<results.end()-1; a++) {
                            //   QBrush blueBrush(Qt::blue);
                            QGraphicsRectItem * path = new QGraphicsRectItem;
                            path->setRect((results[count]->getXPos())*SCALE_FACTOR,(results[count]->getYPos())*SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
                            path->setBrush(Qt::blue);
                            mapscene->addItem(path);
                            pathItem.push_back(path);
                            count ++;
                        }

                        QTimer::singleShot(moveSpeed, this, SLOT(movestep_slot()));
                    } else {
                        if(controll->getenemyInfo().size() != 0) {
                            this->gameOver();
                        } else {
                            this->victroy();
                        }
                    }
                }
            } else {
                ui->terminal->append("Cannot Move!");
                this->gameOver();
            }
        }

    }

}

void MainWindow::specfic_step_slot()
{
    if(getPause() == false&&specific==true) {
        if(results.size()!=0) {
            auto it = results.begin();

            if(controll->movestep((*it)->getXPos(),(*it)->getYPos())) {
                ui->graphicsView->centerOn(controll->getRect());
                this->getStatus();

                if(pathItem.size()!=0) {
                    mapscene->removeItem(pathItem[0]);
                    pathItem.erase(pathItem.begin());
                }

                if(results.size()!=0) {
                    results.erase(results.begin());
                }
                if(results.size()!=0) {
                    QTimer::singleShot(moveSpeed, this, SLOT(specfic_step_slot()));
                } else {
                    ui->btn_goto->setEnabled(true);
                }

            }
        }
    }
}




void MainWindow::on_pushButton_3_clicked()
{
    ui->graphicsView->hide();
    ui->terminal->show();

}

void MainWindow::on_pushButton_4_clicked()
{
    ui->graphicsView->show();
    ui->terminal->hide();

}

void MainWindow::victroy()
{
    std::string str = "Victroy";
    QString qstr=QString::fromStdString(str);
    QMessageBox::information(this,qstr,tr("You win!\nDefeat all enemies"));
}

void MainWindow::gameOver()
{
    std::string str = "Game Over";
    QString qstr=QString::fromStdString(str);
    QMessageBox::information(this,qstr,tr("You are dead!\nNo more health or energy to continue!"));
}

Ui::MainWindow *MainWindow::getUi() const
{
    return ui;
}

void MainWindow::setMoveSpeed(int speed)
{
    moveSpeed = speed*10;

}

void MainWindow::on_btn_goto_clicked()
{
    pause =false;
    specific = true;
    QString qstrx = ui->gotox->text();
    std::string strx = qstrx.toUtf8().constData();
    int goalx = std::stoi(strx);

    QString qstry = ui->gotoy->text();
    std::string stry = qstry.toUtf8().constData();
    int goaly = std::stoi(stry);

    QString qstrStartx = ui->startx->text();
    std::string strStartx = qstrStartx.toUtf8().constData();
    int startx = std::stoi(strStartx);

    QString qstrStarty = ui->starty->text();
    std::string strStarty = qstrStarty.toUtf8().constData();
    int starty = std::stoi(strStarty);

    int index = 0;
    for(auto a=pathItem.begin(); a<pathItem.end(); a++) {
        mapscene->removeItem(pathItem[index]);
        index ++;
    }
    pathItem.clear();
    results.clear();

    controll->specificMove(startx,starty,goalx,goaly);
    results = controll->getPathHero();
    int count = 0;
    results.erase(results.begin());
    for(auto a=results.begin(); a<results.end(); a++) {
        //   QBrush blueBrush(Qt::blue);
        QGraphicsRectItem * path = new QGraphicsRectItem;
        path->setRect((results[count]->getXPos())*SCALE_FACTOR,(results[count]->getYPos())*SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
        path->setBrush(Qt::red);
        mapscene->addItem(path);
        pathItem.push_back(path);
        count ++;
    }
    ui->btn_goto->setEnabled(false);
    specfic_step_slot();
}

void MainWindow::on_btn_magic_clicked()
{
    controll->changeEngrgy(100);
    controll->changeHealth(100);

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    ui->graphicsView->centerOn(controll->getRect());
    if(controll->getHealth()>0&&controll->getEnergy()>0) {
        if(event->key() == Qt::Key_PageUp) {
            controll->move("a");
            if (controll->move("a")) {
                ui->terminal->append("Move left 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else if (event->key() == Qt::Key_PageDown) {
            if (controll->move("d")) {
                ui->terminal->append("Move right 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else if (event->key() == Qt::Key_Up) {
            if (controll->move("w")) {
                ui->terminal->append("Move up 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        } else if (event->key() == Qt::Key_Down) {
            if (controll->move("s")) {
                ui->terminal->append("Move down 1 step");
                //controll->moveMap();
                this->getStatus();
            } else {
                ui->terminal->append("Cannot Move!");
            }
        }
    } else {
        ui->terminal->append("You are dead!");
        this->gameOver();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //pressLabel->setText(QString(tr("點擊處：(%1, %2)")).arg(QString::number(event->x()), QString::number(event->y())));

    int x = event->x();
    int y = event->y();
    qDebug()<<x<<y<<endl;
}

void MainWindow::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{

    qDebug()<<"drag enter"<<endl;
}

void MainWindow::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{

    qDebug()<<"drag leave"<<endl;
}

void MainWindow::dropEvent(QGraphicsSceneDragDropEvent *event)
{

    qDebug()<<"drop"<<endl;
}

void MainWindow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    qDebug()<<"mouse move "<<endl;
}


bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        keyPressEvent((QKeyEvent *)event);
        return true;
    }
    if(event->type()==QEvent::MouseButtonPress) {
        mousePressEvent((QMouseEvent*)event);
        return true;
        //return QObject::eventFilter(object, event);


    } else {
        return QObject::eventFilter(object, event);
    }
}


void MainWindow::resetgame( QString mappath)
{
    pause = true;
    controll->clearEnemy();
    controll->clearHealthPack();
    controll->clearMap();
    pathItem.clear();
    results.clear();

    //TODO delete old controller
    controll = std::make_shared<MapControll>();

    QObject::connect(controll.get(), SIGNAL(energy_value_change(int)), ui->energybar, SLOT(setValue(int)));
    QObject::connect(controll.get(), SIGNAL(health_value_change(int)), ui->healthbar, SLOT(setValue(int)));

    //show the number of enemy and health pack
    QObject::connect(controll.get(),SIGNAL(enmeyNumChanged(int)),ui->enemyNum,SLOT(display(int)));
    QObject::connect(controll.get(),SIGNAL(healthPackNumChanged(int)),ui->healthPackNum,SLOT(display(int)));

    controll->setMapPath(mappath);
    controll->readMap(mappath);
    controll->createMap();

    mapscene = controll->getMapscene();
    ui->graphicsView->setScene(mapscene);
    ui->graphicsView->show();
    ui->graphicsView->centerOn(controll->getRect());
    ui->terminal->append("World created, hero reborn");
    this->getStatus();
    ui->energybar->setValue(100);
    ui->healthbar->setValue(100);
}


void MainWindow::on_pause_clicked()
{
    if(pause == true) {
        pause = false;
        if(specific==false){
        movestep_slot();
        }
        else{
        specfic_step_slot();
        }
    } else {
        pause = true;

    }
}

void MainWindow::on_save_clicked()
{
    SaveSettings();
}

void MainWindow::on_load_clicked()
{
    LoadSettings();
}

void MainWindow::SaveSettings()
{
    QSettings setting("MyApp","mysetting");//name and subname

    setting.beginGroup("MyMap");
    setting.setValue("map",controll->getMapPath());
    setting.endGroup();

    setting.beginGroup("MyEnemy");
    setting.beginWriteArray("EnemySaves");


    for(unsigned int i =0; i<controll->getenemyInfo().size(); ++i) {


        setting.setArrayIndex(i);
        setting.setValue("enemyXPos",controll->getenemyInfo()[i]->getXPos());
        setting.setValue("enemyYPos",controll->getenemyInfo()[i]->getYPos());
        setting.setValue("enemyValue",controll->getenemyInfo()[i]->getValue());

        PEnemy* isP = dynamic_cast<PEnemy*>(controll->getenemyInfo()[i].get());
        if(isP) {
            setting.setValue("penemy",1);//1 means penemy
        } else {
            setting.setValue("penemy",0);
        }

    }
    setting.endArray();
    setting.endGroup();

    setting.beginGroup("MyHealthpack");
    setting.beginWriteArray("HealthpackSaves");

    for(unsigned int i =0; i<controll->getHealthPackInfo().size(); ++i) {

        setting.setArrayIndex(i);
        setting.setValue("HealthpackXPos",controll->getHealthPackInfo()[i]->getXPos());
        setting.setValue("HealthpackYPos",controll->getHealthPackInfo()[i]->getYPos());
        setting.setValue("HealthpackValue",controll->getHealthPackInfo()[i]->getValue());
    }
    setting.endArray();
    setting.endGroup();

    setting.beginGroup("MyHero");
    setting.setValue("HeroXPos",controll->getHeroXPos());
    setting.setValue("HeroYPos",controll->getHeroYPos());
    setting.setValue("HealthValue",controll->getHealth());
    setting.setValue("EnergyValue",controll->getEnergy());
    setting.endGroup();


    qDebug()<<"saved";
}

void MainWindow::LoadSettings()
{

    QSettings setting("MyApp","mysetting");//name and subname

    setting.beginGroup("MyMap");
    resetgame(setting.value("map").toString());
    setting.endGroup();


    setting.beginGroup("MyEnemy");
    int size1 = setting.beginReadArray("EnemySaves");
    std::vector<std::shared_ptr<Enemy>> enemySaveVector;

    for(int i = 0; i < size1; ++i) {
        setting.setArrayIndex(i);
        if(setting.value("penemy").toInt() == 0) {
            std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(0,0,0);
            enemy->setXPos(setting.value("enemyXPos").toInt());
            enemy->setYPos(setting.value("enemyYPos").toInt());
            enemy->setValue(setting.value("enemyValue").toFloat());
            enemySaveVector.push_back(enemy);
        } else {
            std::shared_ptr<PEnemy> enemy = std::make_shared<PEnemy>(0,0,0);
            enemy->setXPos(setting.value("enemyXPos").toInt());
            enemy->setYPos(setting.value("enemyYPos").toInt());
            enemy->setValue(setting.value("enemyValue").toFloat());
            enemySaveVector.push_back(enemy);
        }

    }
    controll->setEnemies(enemySaveVector);
    controll->addEnemyInfo();
    setting.endArray();
    setting.endGroup();

    setting.beginGroup("MyHealthpack");
    int size2 = setting.beginReadArray("HealthpackSaves");
    std::vector<std::shared_ptr<Tile>> healthpackSaveVector;
    for(int i = 0; i < size2; ++i) {
        setting.setArrayIndex(i);
        std::shared_ptr<Tile> healthpack = std::make_shared<Tile>(0,0,0);
        healthpack->setXPos(setting.value("HealthpackXPos").toInt());
        healthpack->setYPos(setting.value("HealthpackYPos").toInt());
        healthpack->setValue(setting.value("HealthpackValue").toFloat());
        healthpackSaveVector.push_back(healthpack);
    }
    controll->setHealthPack(healthpackSaveVector);
    controll->addHealthPackInfo();
    setting.endArray();
    setting.endGroup();

    setting.beginGroup("MyHero");
    controll->setHeroValue(setting.value("HeroXPos").toInt(),setting.value("HeroYPos").toInt(),
                           setting.value("HealthValue").toInt(),setting.value("EnergyValue").toFloat());
    setting.endGroup();


    ui->graphicsView->centerOn(controll->getRect());


    qDebug()<<"Loaded";
}

void MainWindow::enemydefectslot()
{
    ui->terminal->append("A enemy defected");

}








