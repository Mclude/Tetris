#include "gamebox.h"
#include <QDebug>

#include <QGraphicsScene>
#include <QTimer>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

GameBox::GameBox(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    //初始化颜色
    m_color = QColor(0,0,0,255);
    //初始化半径
    m_radius = 7;
}

// 获取颜色属性
QColor GameBox::Color() const
{
    return m_color;
}
// 设置颜色属性
void GameBox::setColor(const QColor &newColor)
{
    m_color = newColor;

    scene()->update(QRect(-160,-183,206,366));
}

//设置动画效果1
void GameBox::setChangeColorAnimated()
{
    ChangeColor = new QPropertyAnimation(this,"color");
    ChangeColor->setDuration(300);
    ChangeColor->setStartValue(QColor(0,0,0,255));
    ChangeColor->setKeyValueAt(0.5,QColor(100,100,100,50));
    ChangeColor->setEndValue(QColor(0,0,0,255));
    ChangeColor->setLoopCount(2);

}
//设置动画效果2
void GameBox::setAlphaColorAnimated()
{

    AlphaColor = new QPropertyAnimation(this,"color");
    AlphaColor->setDuration(80);
    AlphaColor->setStartValue(QColor(0,0,0,50));
    AlphaColor->setEndValue(QColor(0,0,0,255));
    AlphaColor->setLoopCount(1);

}

// ###### GameGroup ######

GameGroup::GameGroup()
{
    m_shape = Random;
    this->T_start = new QTimer(this);
    connect(T_start,&QTimer::timeout,this,&GameGroup::movebyone);

//    oldTransform = transform();
    ParallelAnimat = new QParallelAnimationGroup(this);



}

//判断是否碰撞
bool GameGroup::iscollsion()
{
    QList<QGraphicsItem*> list = childItems();

    foreach(QGraphicsItem *item,list)
    {

        QList<QGraphicsItem*> colllist = scene()->collidingItems(item);
        int n = 0;
        foreach (QGraphicsItem *item2, colllist)
        {
            if(item2->data(1) == "游戏区域")
                return true;
            if(item2->data(1) == "操作方块")
                n++;

        }
        if(n>0)
            return true;

    }
    return false;
}

//创建方块
void GameGroup::curateShape(const QPointF &point,Shape shape)
{

    shapegroup.clear();
    for (int var = 0; var < 4; ++var)
    {
        auto *Box = new GameBox;
        Box->setData(1,"操作方块");
        addToGroup(Box);
        shapegroup.append(Box);
    }

    if(shape == Random)
    {
        int randshape = QRandomGenerator::global()->bounded(7);
        shape = (Shape)randshape;
//        shape = Shape::I;
    }


    switch (shape) {
    case I:
    {
        shapegroup.at(0)->setPos(0,0);
        shapegroup.at(1)->setPos(-m_diameter,0);
        shapegroup.at(2)->setPos(m_diameter,0);
        shapegroup.at(3)->setPos(m_diameter*2,0);
        break;
    }
    case Z:
    {
        shapegroup.at(0)->setPos(-m_diameter,-m_diameter);
        shapegroup.at(1)->setPos(0,-m_diameter);
        shapegroup.at(2)->setPos(0,0);
        shapegroup.at(3)->setPos(m_diameter,0);
        break;
    }
    case S:
    {
        shapegroup.at(0)->setPos(0,0);
        shapegroup.at(1)->setPos(-m_diameter,0);
        shapegroup.at(2)->setPos(0,-m_diameter);
        shapegroup.at(3)->setPos(m_diameter,-m_diameter);
        break;
    }
    case J:
    {
        shapegroup.at(0)->setPos(0,0);
        shapegroup.at(1)->setPos(-m_diameter,0);
        shapegroup.at(2)->setPos(m_diameter,0);
        shapegroup.at(3)->setPos(m_diameter,-m_diameter);
        break;
    }
    case L:
    {
        shapegroup.at(0)->setPos(0,0);
        shapegroup.at(1)->setPos(-m_diameter,0);
        shapegroup.at(2)->setPos(m_diameter,0);
        shapegroup.at(3)->setPos(m_diameter,-m_diameter);
        break;
    }

    case O:
    {
        shapegroup.at(0)->setPos(0,0);
        shapegroup.at(1)->setPos(m_diameter,0);
        shapegroup.at(2)->setPos(0,-m_diameter);
        shapegroup.at(3)->setPos(m_diameter,-m_diameter);
        break;
    }
    case T:
    {
        shapegroup.at(0)->setPos(0,0);
        shapegroup.at(1)->setPos(-m_diameter,0);
        shapegroup.at(2)->setPos(m_diameter,0);
        shapegroup.at(3)->setPos(0,-m_diameter);
        break;
    }
    default:
        break;
    }
    setPos(point);

    m_shape = shape;
}

// 消除方块
bool GameGroup::clearBoxGroup(bool is_delet)
{

    shapegroup.clear();
    ParallelAnimat->clear();

    QList<QGraphicsItem *> list = childItems();
    //如果 返回true 游戏结束
    if(iscollsion())
    {
        emit gameover();
        return true;
    }
    for(QGraphicsItem *item : list)
    {
      removeFromGroup(item);
      if(is_delet)
      {
        scene()->removeItem(item);
      }
    }
    emit needBox();

    return false;
}


// 开启定时器
void GameGroup::statrTimer(int interval)
{
    T_start->start(interval);
}

//停止定时器
void GameGroup::stopTimer()
{
    T_start->stop();
}

//向下移动一格
void GameGroup::movebyone()
{
    moveBy(0,20);
    if(iscollsion())
    {
        moveBy(0,-20);
        //如果 返回true 游戏结束
        clearBoxGroup(false);
    }
}

//旋转
void GameGroup::myRotate(bool direction)
{
    if(shapegroup.isEmpty())
        return;

    QPoint point0;
    QPoint point1;
    QPoint point2;
    QPoint point3;

    if(direction)
    {
        //逆时针
        point0.setX(shapegroup.at(0)->y());
        point0.setY(-shapegroup.at(0)->x());

        point1.setX(shapegroup.at(1)->y());
        point1.setY(-shapegroup.at(1)->x());

        point2.setX(shapegroup.at(2)->y());
        point2.setY(-shapegroup.at(2)->x());

        point3.setX(shapegroup.at(3)->y());
        point3.setY(-shapegroup.at(3)->x());

    }
    else
    {
        //顺时针
        point0.setX(-shapegroup.at(0)->y());
        point0.setY(shapegroup.at(0)->x());

        point1.setX(-shapegroup.at(1)->y());
        point1.setY(shapegroup.at(1)->x());

        point2.setX(-shapegroup.at(2)->y());
        point2.setY(shapegroup.at(2)->x());

        point3.setX(-shapegroup.at(3)->y());
        point3.setY(shapegroup.at(3)->x());
    }


    shapegroup.at(0)->setPos(point0);
    shapegroup.at(1)->setPos(point1);
    shapegroup.at(2)->setPos(point2);
    shapegroup.at(3)->setPos(point3);
    update();
}

//开启动画
void GameGroup::starAnimat()
{
    this->ParallelAnimat->start();
}





