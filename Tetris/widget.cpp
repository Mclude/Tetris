#include "widget.h"
#include "qsoundeffect.h"
#include "ui_widget.h"

#include "gameview.h"
#include "gamebox.h"

//#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    //设置为无边框
    setWindowFlags(Qt::FramelessWindowHint);
    //设置背景透明
    setAttribute(Qt::WA_TranslucentBackground,true);
    auto *show = new QGraphicsDropShadowEffect(this);
    show->setOffset(4);
    show->setColor(QColor("#444444"));
    show->setBlurRadius(22);
    this->setGraphicsEffect(show);


    ui->setupUi(this);
    Backcolor = QColor(0,0,0,0);
    gameView = new GameView(this);
    Backcolor = QColor("#A2D8F4");  //背景颜色
    initGameBtn();
    //窗口大小
    setMinimumSize(650,1100);
    resize(650,1100);
    setWindowIcon(QIcon(":/img/icon.png"));

}

Widget::~Widget()
{
    delete ui;
}

//*
void Widget::paintEvent(QPaintEvent *event)
{

    auto *painter = new QPainter(this);

    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter->save();
    //填充背景
    painter->translate(width()/2,height()/2);

    //游戏机主体
    deawGameRect(painter);

    //按键提示文字
    drawbtnText(painter);
    //按键提示文字箭头
    drawbtnTextArrowKey(painter);

    painter->restore();
    painter->end();

    QWidget::paintEvent(event);

}

void Widget::deawGameRect(QPainter *painter)
{
    //游戏机背景
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor("#efcc1a")));
    painter->drawRoundedRect(-300,-490,600,980,22,22);

    //黑色框
    painter->translate(0,-180);
    QPen pen(QColor(Qt::black),10);
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    int rectw = 450;
    int recth = 490;
    painter->drawLine( QLine( -225+50,   -255 ,      -225,          -255 ) );
    painter->drawLine( QLine( -225,      -255 ,      -225,          -255+recth ) );
    painter->drawLine( QLine( -225,      -255+recth, -225+rectw,    -255+recth ) );
    painter->drawLine( QLine( -225+rectw,-255+recth, -225+rectw,    -255 ) );
    painter->drawLine( QLine( -225+rectw,-255,       -225+rectw-50, -255 ) );
    //标题
    painter->setFont(QFont("微软雅黑",15));
    painter->drawText(QRect(-225,-305,450,100),Qt::AlignCenter,"俄罗斯方块");
    //点
    QPolygon points;
    points<<QPoint(-225+70,-255)<<QPoint(-225+90,-255)<<QPoint(-225+110,-255)<<QPoint(-225+130,-255);
    painter->drawPoints(points);
    painter->translate(250,0);
    painter->drawPoints(points);
    painter->restore();

    //两边的方块
    drawbox(painter);

}

void Widget::drawbox(QPainter *painter)
{
    painter->save();
    QPixmap map(":/img/box.jpg");
    map = map.scaled(map.width()*0.6,map.height()*0.6);
    painter->translate(-285,-420);
    painter->drawPixmap(map.rect(),map);

    QPixmap rmap(":/img/box2.jpg");
    rmap = rmap.scaled(rmap.width()*0.6,rmap.height()*0.6);
    painter->translate(530,0);
    painter->drawPixmap(rmap.rect(),rmap);
    painter->restore();
}

void Widget::drawKeyShow(QPainter *painter)
{

    //空格
    painter->save();
    painter->translate(-500,400);
    QRectF space;
    space.setRect(-175,-40,350,80);
    painter->setBrush(QColor(55,77,75,200));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(space,10,10);
    //文字

    QPen pen;
    pen.setColor(QColor(255,255,255,150));
    painter->setPen(pen);
    painter->setFont(QFont("微软雅黑",15));
    painter->drawText(space,Qt::AlignCenter,"SPACE");
    painter->restore();

    // 方向键
    painter->save();
    painter->translate(500,415);
    painter->setFont(QFont("微软雅黑",15));
    painter->setBrush(QColor(55,77,75,150));
    painter->setPen(Qt::NoPen);
    QRectF direction;
    direction.setRect(-50,-25,100,50);
    painter->drawRoundedRect(direction,10,10);

    direction.setRect(-50-120,-25,100,50);
    painter->drawRoundedRect(direction,10,10);

    direction.setRect(-50+120,-25,100,50);
    painter->drawRoundedRect(direction,10,10);

    direction.setRect(-50,-35-70,100,70);
    painter->drawRoundedRect(direction,10,10);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255,255,255,150));

    // ↓
    QPolygonF points;
    points << QPointF(-10, -8) << QPointF(10, -8) << QPointF(0, 8);

    painter->drawPolygon(points);
    // ↑
    painter->save();
    painter->translate(0,-70);
    painter->rotate(180);
    painter->drawPolygon(points);
    painter->restore();
    // ←
    painter->save();
    painter->translate(-120,0);
    painter->rotate(90);
    painter->drawPolygon(points);
    painter->restore();
    // →
    painter->save();
    painter->translate(120,0);
    painter->rotate(-90);
    painter->drawPolygon(points);
    painter->restore();
    painter->restore();

}

void Widget::drawbtnText(QPainter *painter)
{
    painter->save();
    painter->translate(-225,145);
    painter->setFont(QFont("微软雅黑",10));
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"暂停(P)");

    painter->translate(80,0);
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"音效(S)");

    painter->translate(80,0);
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"重玩(R)");
    painter->restore();

    painter->save();
    painter->translate(-145,360);
    painter->setFont(QFont("微软雅黑",15));
    painter->drawText(QRect(-100,-30,200,60),Qt::AlignCenter,"掉落(SPACE)");
    painter->restore();

    painter->save();
    painter->setFont(QFont("微软雅黑",10));
    painter->translate(195,105);
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"旋转");

    painter->translate(-70,280);
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"下移");

    painter->translate(-85,-90);
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"左移");
    painter->translate(185,0);
    painter->drawText(QRect(-50,-15,100,30),Qt::AlignCenter,"右移");

    painter->restore();

}

void Widget::drawbtnTextArrowKey(QPainter *painter)
{
    // ↑
    QPolygonF points;
    painter->save();
    painter->setBrush(QBrush(Qt::black));
    painter->translate(130,220);

    points << QPointF(-8, -8) << QPointF(8, -8) << QPointF(0, -24);
    painter->drawPolygon(points);
    painter->save();
    painter->translate(0,20);
    painter->rotate(180);
    painter->drawPolygon(points);
    painter->restore();

    painter->save();
    painter->translate(-10,10);
    painter->rotate(-90);
    painter->drawPolygon(points);
    painter->restore();

    painter->save();
    painter->translate(10,10);
    painter->rotate(90);
    painter->drawPolygon(points);
    painter->restore();

    painter->restore();
}

//初始化 按钮 游戏界面
void Widget::initGameBtn()//游戏机按钮
{
    int diameter = 50; //直径
    ui->PauseBtn->resize(diameter,diameter);
    ui->MusicBtn->resize(diameter,diameter);
    ui->ReStartBtn->resize(diameter,diameter);
    diameter = 160;
    ui->DropOutBtn->resize(diameter,diameter);
    diameter = 100;
    ui->RotateBtn->resize(diameter,diameter);
    ui->MoveLeftBtn->resize(diameter,diameter);
    ui->MoveRightBtn->resize(diameter,diameter);
    ui->MoveDownBtn->resize(diameter,diameter);

    //设置快捷键
    ui->PauseBtn->setShortcut(Qt::Key_P);
    ui->MusicBtn->setShortcut(Qt::Key_S);
    ui->ReStartBtn->setShortcut(Qt::Key_R);

    ui->DropOutBtn->setShortcut(Qt::Key_Space);

    ui->RotateBtn->setShortcut(Qt::Key_Up);
    ui->MoveLeftBtn->setShortcut(Qt::Key_Left);
    ui->MoveRightBtn->setShortcut(Qt::Key_Right);
    ui->MoveDownBtn->setShortcut(Qt::Key_Down);
    //连接槽函数
    connect(ui->PauseBtn,&QPushButton::pressed,[=]{gameView->on_press_btn();gameView->parusebtn->play(); });
    connect(ui->MusicBtn,&QPushButton::pressed,[=]{gameView->on_sound_btn();gameView->button->play(); });
    connect(ui->ReStartBtn,&QPushButton::pressed,[=]
            {
                if(gameView->is_start)
                {
                    gameView->GameOver();
                    gameView->button->play();
                }
            });

    connect(ui->DropOutBtn,&QPushButton::pressed,[=]{gameView->on_Space_btn();gameView->downbtn->play(); });

    connect(ui->RotateBtn,&QPushButton::pressed,[=]{gameView->on_up_btn();});
    connect(ui->MoveLeftBtn,&QPushButton::pressed,[=]{gameView->on_left_btn();gameView->button->play(); });
    connect(ui->MoveRightBtn,&QPushButton::pressed,[=]{gameView->on_right_btn();gameView->button->play(); });
    connect(ui->MoveDownBtn,&QPushButton::pressed,[=]{gameView->on_down_btn();gameView->button->play(); });
}

//事件
void Widget::resizeEvent(QResizeEvent *e)
{
    int w  = width();
    int h = height();
        //按钮的位置
    int tx = w/2-250;
    int ty = h/2+80;
    ui->PauseBtn->move(tx,ty);
    ui->MusicBtn->move(tx+80,ty);
    ui->ReStartBtn->move(tx+160,ty);

    ui->DropOutBtn->move(w/2-230,h/2+180);

    int x = w/2+80;
    int y = h/2+90;
    ui->RotateBtn->move(x,y);
    ui->MoveLeftBtn->move(x-90,y+90);
    ui->MoveRightBtn->move(x+90,y+90);
    ui->MoveDownBtn->move(x,y+180);

    ui->close->move(w/2+240,h/2-480);
    ui->minwindow->move(w/2+200,h/2-480);

    //视图位置
    gameView->move(w/2-175,h/2-385);

    QWidget::resizeEvent(e);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QPoint y = event->globalPos(); //鼠标在桌面的位置
    QPoint x = geometry().topLeft();//窗口左上角的位置
    z = y - x;
    QWidget::mousePressEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    z = QPoint();
    QWidget::mouseReleaseEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint y = event->globalPos(); //鼠标在桌面的位置
    QPoint x = y - z;//窗口左上角的位置
    move(x);

    QWidget::mouseMoveEvent(event);
}

