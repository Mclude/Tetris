#include "gameview.h"
#include "gamebox.h"

#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QRandomGenerator>

//游戏的初始速度
static const int INITSPEED = 800;

GameView::GameView(QWidget *parent)
    : QGraphicsView{parent}
{
    //变量初始化
    is_Perss = false;
    is_start = false;
    is_over = true;
    is_sound = true;
    point = QPoint(QPoint(-27-40,-150));
    gameScene = new QGraphicsScene(this);
    nextBox = new GameGroup;
    thisBox = new GameGroup;
    S_AnimationGp = new QSequentialAnimationGroup;

    initinterface();
    initMusic();

    //启动动画
    S_AnimationGp->start();
    QTimer::singleShot(3000,[=]{
        S_AnimationGp->setDirection(QAbstractAnimation::Backward);
        S_AnimationGp->start();
        QTimer::singleShot(1600,[=]{
            S_AnimationGp->setDirection(QAbstractAnimation::Forward);
            Title->show();      //显示标题
            auto *show = new QGraphicsDropShadowEffect(this);
            show->setOffset(4);
            show->setColor(QColor("#444444"));
            show->setBlurRadius(8);
            auto *Overanimation = new QPropertyAnimation(Title,"y");
            Title->setGraphicsEffect(show);
            Overanimation->setDuration(1000);
            Overanimation->setEasingCurve(QEasingCurve::OutBounce);
            Overanimation->setStartValue(-500);
            Overanimation->setEndValue(Over->y());
            Overanimation->start();
            is_over = false;  //等待启动动画结束 才能开始游戏

        });
    });

    //暂停 音效 图标闪烁定时器
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&GameView::glint);
    timer->start(500);


}

void GameView::initinterface()//游戏机屏幕
{

    //设置视图
    setScene(gameScene);

    int view_W = 350;
    int view_H = 390;
    resize(view_W,view_H);
        //边框 凹陷效果
    setStyleSheet(
    "border:5px inset;"
        "border-left-color:rgb(153,127,16,200);"
        "border-top-color:rgb(153,127,16,200);"
        "border-bottom-color:rgb(249,227,108,150);"
        "border-right-color:rgb(249,227,108,150)");
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setRenderHints(QPainter::Antialiasing);
    setSceneRect(-(view_W-10)/2,-(view_H-10)/2,view_W-10,view_H-10);

    //设置场景
    gameScene->setBackgroundBrush(QBrush(QColor("#9ead86")));
    gameScene->addItem(thisBox);
    gameScene->addItem(nextBox);
    connect(thisBox,&GameGroup::needBox,this,&GameView::clearFullRow);
    connect(thisBox,&GameGroup::gameover,this,&GameView::GameOver);

    // 初始化游戏操作区
    initbackbound();

    //初始化提示类文字
    initGamehint();

}

void GameView::initbackbound()//屏幕背景
{
    //游戏框
    qreal W = 206;
    qreal H = 366;
    qreal X = -W/2-57; //-160
    qreal Y = -H/2;   //-183
    qreal interval = 6;

    auto *LineTop = new QGraphicsLineItem(QLine(X,Y,X+W,Y));
    auto *LineDown = new QGraphicsLineItem(QLine(X,Y+H,X+W,Y+H));
    auto *LineLift = new QGraphicsLineItem(QLine(X,Y,X,Y+H));
    auto *LineRight = new QGraphicsLineItem(QLine(X+W,Y,X+W,Y+H));

    LineTop->setPen(QPen(Qt::black,3));
    LineDown->setPen(QPen(Qt::black,3));
    LineLift->setPen(QPen(Qt::black,3));
    LineRight->setPen(QPen(Qt::black,3));

    LineTop->setData(1,"游戏区域");
    LineDown->setData(1,"游戏区域");
    LineLift->setData(1,"游戏区域");
    LineRight->setData(1,"游戏区域");

    gameScene->addItem(LineTop);
    gameScene->addItem(LineDown);
    gameScene->addItem(LineLift);
    gameScene->addItem(LineRight);

    // 游戏框内 半透明方块 背景  由 10 * 18 的小方块组成
    //设置 动画效果 在游戏启动 或者 游戏结束时 让方块 改变透明度浮上来
    QList<QList<GameBox*>> list;
    //创建
    for (int row = 17; row >= 0; row--)
    {
        QList<GameBox*> box;//保存每行的方块
        auto *AnimationGp = new QParallelAnimationGroup(this);//创建并行播放组
        for (int column = 0; column < 10; ++column)
        {
            auto *game = new GameBox;
            gameScene->addItem(game);
            game->setData(1,"背景");
            game->setEnabled(false);
            game->setColor(QColor(0,0,0,50));
            box.append(game);

            game->setAlphaColorAnimated(); //设置动画
            AnimationGp->addAnimation(game->AlphaColor);  //添加并行播放组
        }
        //把每行的并行动画添加到视图的串行动画组
        S_AnimationGp->addAnimation(AnimationGp);

        list.append(box);
        qreal radius = box.at(0)->getRadius(); //半径
        qreal diameter = radius*2;  //直径

        //设置位置
        for (int column = 0; column < 10; ++column)
        {
            box.at(column)->setPos(
            X+radius+interval+(diameter+interval)*column,
            Y+radius+interval+(diameter+interval)*row);
            // x = (框的X + 半径 + 间隔) + (直径 + 间隔) * 列
            // Y = (框的Y + 半径 + 间隔) + (直径 + 间隔) * 行
        }
    }

}

void GameView::initGamehint()//提示类文字
{
    //用到的字体
    QFont font("微软雅黑",10);
    QFont font2("Fira Code SemiBold",20);

    font2.setBold(true);
    font2.setLetterSpacing(QFont::AbsoluteSpacing,5);
    font2.setItalic(true);
    //分数文字提示
    ScoreText = gameScene->addSimpleText("最高分",font);
    ScoreText->setPos(60,-170);

    //分数  显示三种分数(最高分、当前分、上轮分)
    Score = gameScene->addSimpleText(QString("%1").arg(topscore),font);
    Score->setPos(110,-150);

    //未在游戏中就交替显示 最高分 与 上轮分数  游戏中则显示当前分数
    auto *timer = new QTimer(this);
    timer->start(4000);
    QTimer::singleShot(2000,[=]{
        ScoreText->setText("上轮分数");
        Score->setText(QString("%1").arg(lastscore));
    });
    connect(timer,&QTimer::timeout,[=]{
        if(is_start)
            return;
        ScoreText->setText("最高分");
        Score->setText(QString("%1").arg(topscore));
        QTimer::singleShot(2000,[=]{
            if(is_start)
                return;
            ScoreText->setText("上轮分数");
            Score->setText(QString("%1").arg(lastscore));
        });
    });

    //起始行与已消行 的文字提示
    InitialRowText = gameScene->addSimpleText("起始行",font);
    InitialRowText->setPos(60,-110);
    //起始行
    InitialRow = gameScene->addSimpleText(QString("%1").arg(initialrow),font);
    InitialRow->setPos(110,-90);


    //级别的文字提示
    LevelText = gameScene->addSimpleText("级别",font);
    LevelText->setPos(60,-40);
    //级别
    Level = gameScene->addSimpleText(QString("%1").arg(level),font);
    Level->setPos(110,-20);


    //下一个 方块提示文字
    auto nextText = gameScene->addSimpleText("下一个",font);
    nextText->setPos(60,20);
    //创建提示方块
    nextBox->curateShape(QPoint(100,80));

    //游戏标题提示文字
    Title = gameScene->addText("Tetris",font2);

    Title->setDefaultTextColor("#444444");
    Title->setPos(-140,-60);
    Title->hide();
    //游戏失败提示文字
    Over = gameScene->addText("OVER",font2);
    Over->setDefaultTextColor("#444444");
    Over->setPos(-113,-60);
    gameScene->addItem(Over);
    Over->hide();
}

void GameView::initMusic()//游戏音效初始化
{
    //启动音效 游戏开始后替换成 游戏失败音乐
    stataSound = new QSoundEffect;
    stataSound->setSource(QUrl::fromLocalFile(":/img/stara.wav"));
    stataSound->setLoopCount(1);
    stataSound->play();

    //清除满行音效
    clearRowSound = new QSoundEffect;
    clearRowSound->setSource(QUrl::fromLocalFile(":/img/clear.wav"));
    clearRowSound->setLoopCount(1);
    clearRowSound->setVolume(0.1);

    //强落音效
    downbtn = new QSoundEffect;
    downbtn->setSource(QUrl::fromLocalFile(":/img/show.wav"));
    downbtn->setLoopCount(1);
    downbtn->setVolume(0.1);

    //背景音乐
    backSound = new QSoundEffect;
    backSound->setSource(QUrl::fromLocalFile(":/img/backmusic.wav"));
    backSound->setVolume(1);
    backSound->setLoopCount(QSoundEffect::Infinite);

    //旋转音效
    roate = new QSoundEffect;
    roate->setSource(QUrl::fromLocalFile(":img/rotate_[cut_0sec].wav"));
    roate->setLoopCount(1);
    roate->setVolume(0.3);

    //按键音效
    button = new QSoundEffect;
    button->setSource(QUrl::fromLocalFile(":/img/btn.wav"));
    button->setLoopCount(1);
    button->setVolume(0.8);

    //暂停音效
    parusebtn = new QSoundEffect;
    parusebtn->setSource(QUrl::fromLocalFile(":/img/parss.wav"));
    parusebtn->setLoopCount(1);

    //音效图标
    soundIcon = gameScene->addPixmap(QPixmap(":/img/soundclose.png"));
    soundIcon->setScale(0.8);
    soundIcon->setPos(60,150);
    soundIcon->hide();

    pauseIcon = gameScene->addPixmap(QPixmap(":/img/pause.png"));
    pauseIcon->setPos(90,150);
    pauseIcon->setScale(0.8);

    pauseIcon->hide();
}

void GameView::glint()//定时器 闪烁图标
{
    if(is_Perss)
    {
        if(!pauseIcon->isVisible())
            pauseIcon->show();
        else
            pauseIcon->hide();
    }
    else
        pauseIcon->hide();

    if(!is_sound)
    {
        if(!soundIcon->isVisible())
            soundIcon->show();
        else
            soundIcon->hide();
    }
    else
        soundIcon->hide();

}

void GameView::startGame()//游戏开始
{
    //如果游戏正在结束中无法开始游戏
    if(is_over)
        return;
    //文字动画
    if(Over->isVisible())
    {
        auto *Overanimation = new QPropertyAnimation(Over,"y");
        Overanimation->setDuration(1000);
        Overanimation->setStartValue(-60);
        Overanimation->setEndValue(-500);
        Overanimation->start();
    }
    if(Title->isVisible())
    {
        auto *Overanimation = new QPropertyAnimation(Title,"scale");
        Overanimation->setDuration(300);
        Overanimation->setStartValue(1);
        Overanimation->setEndValue(0.1);
        Overanimation->start();
        QTimer::singleShot(300,[=]{Title->hide();});
    }

    backSound->play();
    //把音源改成 结束的音源
    stataSound->setSource(QUrl::fromLocalFile(":/img/over.wav"));
    stataSound->setVolume(0.1);

    //当前分数清0
    newcore = 0;
    //当前已清除行清0
    clearrows = 0;

    //更改 一些提示
    //显示当前分数
    ScoreText->setText("当前分数");
    Score->setText(QString("%1").arg(newcore));
    //显示已清行数
    InitialRowText->setText("清除行数");
    InitialRow->setText(QString("%1").arg(clearrows));


    thisBox->curateShape(point,nextBox->getshape());

    initLevel();    //设置等级
    initialRow();   //设置起始行
    thisBox->statrTimer(gamespeed);

    nextBox->clearBoxGroup(true);
    nextBox->curateShape(QPoint(100,80));

    is_start = true;
    is_Perss = false;

}

void GameView::GameOver()//游戏结束
{
    is_over = true;

    //播放音效
    stataSound->play();
    backSound->stop();

    is_start = false;
    is_Perss = false;

    thisBox->stopTimer();

    //当局游戏获得分数不为0则添加到 list
    if(newcore!=0)
        coreList.append(newcore);
    lastscore = newcore;
    //与 list 的分数比较 更新最高的分数
    for(int core : coreList)
    {
        if(core>topscore)
            topscore = core;
    }

    //显示初始行
    InitialRowText->setText("初始行");
    InitialRow->setText(QString("%1").arg(initialrow));

    //启动动画
    S_AnimationGp->start();
    QTimer::singleShot(3000,[=]{
        S_AnimationGp->setDirection(QAbstractAnimation::Backward);
        S_AnimationGp->start();
        //删除方块
        QList<QGraphicsItem *> list = gameScene->items(QRectF(-160,-220,206,400),Qt::ContainsItemShape);
        if(!list.empty())
        {
           for(QGraphicsItem* item : list)
            {
                if(item->data(1) == "操作方块")
                    gameScene->removeItem(item);
            }
        }
        QTimer::singleShot(1600,[=]{
            S_AnimationGp->setDirection(QAbstractAnimation::Forward);
            is_over = false;
            Over->show();
            auto *show = new QGraphicsDropShadowEffect(this);
            show->setOffset(4);
            show->setColor(QColor("#444444"));
            show->setBlurRadius(8);
            auto *Overanimation = new QPropertyAnimation(Over,"y");
            Over->setGraphicsEffect(show);
            Overanimation->setDuration(1000);
            Overanimation->setEasingCurve(QEasingCurve::OutBounce);
            Overanimation->setStartValue(-500);
            Overanimation->setEndValue(-60);
            Overanimation->start();
            });
    });

}

void GameView::clearFullRow()//清除满行
{
    downbtn->play();
    for (int i = 0; i <= 17; i++)
    {
        QList<QGraphicsItem *> list = gameScene->items(QRectF(-160,-180+(20*i),206,20),Qt::ContainsItemShape);

        if(list.count() == 20)
        {

            // 保存满行的位置
            rows << -180 + (i*20);

            auto *group = new QParallelAnimationGroup(this);
            //删除行 添加动画
            for(QGraphicsItem *item : list)
            {
                if(item->data(1) == "背景")
                    continue;
                GameBox *box = (GameBox*) item;
//                box->deleteLater();
                auto * blur = new QGraphicsBlurEffect;
                box->setGraphicsEffect(blur);
                auto * amimation = new QPropertyAnimation(box,"scale");
                amimation->setDuration(150);
                amimation->setLoopCount(1);
                amimation->setStartValue(1);
                amimation->setKeyValueAt(0.5,1.5);
                amimation->setEndValue(0.1);
                group->addAnimation(amimation);
                QTimer::singleShot(300,[=]{box->deleteLater();});

            }
            group->start();

        }
    }
    updateCore(rows.size());  //更新分数
    updateLeve();       //更新等级
    if(rows.count() > 0)
    {
        //更新清除行数
        clearrows += rows.size();
        InitialRow->setText(QString("%1").arg(clearrows));

        clearRowSound->play();
        //下移 行
        QTimer::singleShot(300,this,&GameView::downBox);

    }
    else
    {
        // 创建下一个方块
        thisBox->curateShape(point,nextBox->getshape());

        //删除 方块 重新创建新的下一个方块
        nextBox->clearBoxGroup(true);
        nextBox->curateShape(QPoint(100,80));
    }


}

void GameView::downBox()//下移 满行的上方方块
{

    for(int i : rows)
    {
//        qDebug()<<rows.at(i);
//        auto *group = new QParallelAnimationGroup(this);
        QList<QGraphicsItem *> list = gameScene->items(QRectF(-160,-180,206,i+180),Qt::ContainsItemShape);
        for(QGraphicsItem *item : list)
        {
            if(item->data(1) == "背景")
                continue;
            item->moveBy(0,20);
//            GameBox *box = (GameBox*) item;
//            auto *animation = new QPropertyAnimation(box,"y");
//            animation->setDuration(10);
//            animation->setStartValue(box->y());
//            animation->setEndValue(box->y()+20);
//            group->addAnimation(animation);
        }
//        group->start();

    }

    rows.clear();
    // 创建下一个方块
    thisBox->curateShape(point,nextBox->getshape());
    //删除 方块 重新创建新的下一个方块
    nextBox->clearBoxGroup(true);
    nextBox->curateShape(QPoint(100,80));



}

void GameView::initLevel()// 初始化level
{
    gameleve = level;
    gamespeed = INITSPEED-(gameleve*90);

}

void GameView::initialRow()// 初始化InitialRow
{    


    for(int i = 0;i < initialrow; i++)
    {
        QList<GameBox*> boxlist;//保存每行的方块
        //创建一行方块
        for (int column = 0; column < 10; ++column)
        {
            auto *box = new GameBox;
            box->setData(1,"操作方块");
            box->setPos(
                -160+7+6+(20)*column,
                -183+7+6+(20)*(17-i)
                        );
            gameScene->addItem(box);
            boxlist.append(box);
        }
        //随机删除 1 到 2 个方块
        int random = QRandomGenerator::global()->bounded(2) +1;
        for (int var = 0; var < random; ++var) {
            int skipNub = QRandomGenerator::global()->bounded(10);
            boxlist.at(skipNub)->deleteLater();
        }

    }


}

void GameView::updateLeve()// 更新 leve
{
    switch (newcore/800) {
    case 1:
        gameleve = 2;
        break;
    case 2:
        gameleve = 3;
        break;
    case 3:
        gameleve = 4;
        break;
    case 4:
        gameleve = 5;
        break;
    case 5:
        gameleve = 6;
        break;
    default:
        break;
    }
    for(QGraphicsItem *item: gameScene->items(QRectF(50,-183,120,200)))
    {
        if(item->data(1) == "Level")
        {
            qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item)->setText(QString("%1").arg(gameleve));
        }
    }
    gamespeed = INITSPEED-(gameleve*90);
    thisBox->statrTimer(gamespeed);
}

void GameView::updateCore(int rowNub)//更新 score
{
    if(rowNub == 0 )
        newcore+= gameleve*3 +4;
    else
        newcore+= 100 * (rowNub*2-1);

    for(QGraphicsItem *item: gameScene->items(QRectF(50,-183,120,200)))
    {
        if(item->data(1) == "Score")
        {
            qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item)->setText(QString("%1").arg(newcore));
            return;
        }
    }

}

//暂停 开始
void GameView::on_press_btn()
{

    //如果游戏未开始则开始游戏
    if(is_start == false)
        startGame();
    else
    {
        //暂停游戏
        if(is_Perss == false)
        {
            is_Perss = true;
            backSound->stop();
            thisBox->stopTimer();
            return;
        }
        //继续游戏
        is_Perss = false;
        backSound->play();
        thisBox->statrTimer(gamespeed);

    }


}

// 下落
void GameView::on_Space_btn()
{
    //如果游戏未开始则开始游戏
    if(is_start == false || is_Perss)
    {
        if(is_Perss)
        {
            is_Perss = !is_Perss;
            thisBox->statrTimer(gamespeed);
            backSound->play();
            return;
        }
        startGame();
        return;
    }

    while (!thisBox->iscollsion())
    {
        thisBox->moveBy(0,20);

    }
    thisBox->moveBy(0,-20);
    thisBox->clearBoxGroup(false);

}

// 旋转
void GameView::on_up_btn()
{

    if(!is_start || is_Perss || thisBox->getshape() == GameGroup::O)
    {
        if(!is_start)
        {
            initialrow += 1;
            initialrow = initialrow % 11;
            InitialRow->setText( QString( "%1" ).arg( initialrow ) );
            button->play();
        }
        return;
    }

    thisBox->myRotate(false);
    if(thisBox->iscollsion())
        thisBox->myRotate(true);

    roate->play();


}

// 左移
void GameView::on_left_btn()
{

    if(!is_start || is_Perss)
    {
        if(!is_start)
        {
           level = level % 6;
           level -= 1;
           if(level<=0) level+=6;
           Level->setText(QString("%1").arg(level));
        }
        return;
    }
    this->thisBox->moveBy(-20,0);
    if(thisBox->iscollsion())
    {
        this->thisBox->moveBy(20,0);
    }


}

//右移
void GameView::on_right_btn()
{

    if(!is_start || is_Perss)
    {
        if(!is_start)
        {
            level = level % 6;
            level += 1;
            Level->setText(QString("%1").arg(level));
        }
        return;
    }
    this->thisBox->moveBy(20,0);
    if(thisBox->iscollsion())
        this->thisBox->moveBy(-20,0);


}

//下移
void GameView::on_down_btn()
{

    if(!is_start || is_Perss)
    {
        if(!is_start)
        {
            initialrow -=1;
            initialrow = initialrow % 11;
            if(initialrow<0) initialrow += 11;
            InitialRow->setText(QString("%1").arg( initialrow ) );
        }
        return;
    }
    thisBox->moveBy(0,20);
    if(thisBox->iscollsion())
    {
        thisBox->moveBy(0,-20);
        thisBox->clearBoxGroup(false);
    }
}

//音效
void GameView::on_sound_btn()
{

    is_sound = !is_sound;
    if(!is_sound)
    {
        button->setMuted(true);
        roate->setMuted(true);
        parusebtn->setMuted(true);

        downbtn->setMuted(true);
        backSound->setMuted(true);
        stataSound->setMuted(true);
        clearRowSound->setMuted(true);
    }
    else
    {
        button->setMuted(false);
        roate->setMuted(false);
        parusebtn->setMuted(false);
        downbtn->setMuted(false);
        backSound->setMuted(false);
        stataSound->setMuted(false);
        clearRowSound->setMuted(false);
    }
}

