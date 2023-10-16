#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

class GameGroup;
class QParallelAnimationGroup;
class QSequentialAnimationGroup;
class QSoundEffect;
class QMediaPlayer;
class QMediaPlaylist;
class QSound;


class GameView :public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QWidget *parent = nullptr);


    void startGame();               //游戏开始
    void GameOver();                //游戏结束
    void clearFullRow();            //清除满行创建下一个方块
    void downBox();                 //下移行

    void initLevel();               //初始化等级
    void initialRow();              //初始化起始行
    void updateCore(int rowNub);    //更新分数
    void updateLeve();              //更新等级

public:
    void on_press_btn();
    void on_sound_btn();
    void on_Space_btn();
    void on_up_btn();
    void on_left_btn();
    void on_right_btn();
    void on_down_btn();



public:
    QSequentialAnimationGroup *S_AnimationGp;   //背景方块 显现动画

    GameGroup *thisBox;  //正在移动的方块组
    GameGroup *nextBox;  //下一个方块组
    bool is_Perss;      //游戏是否暂停
    bool is_start;      //游戏是否开始
    bool is_over;       //游戏是否正在结束动画


    QPoint point;           //方块起始位置
    int initialrow = 0;     //起始行
    int level= 1;           //初始级别
    int topscore = 0;       //最高分
    int newcore = 0;        //当前分数
    int lastscore = 0;      //上一轮分数
    int clearrows;          //已清除行数
    int gameleve;           //开始游戏后级别

    QVector<int> coreList;  //保存分数容器

    //音效
    QSoundEffect *stataSound;       //启动与游戏结束音效
    QSoundEffect *clearRowSound;    //清除满行音效
    QSoundEffect *downbtn;          //强落下音效
    QSoundEffect *backSound;        //背景音乐
    QSoundEffect *parusebtn;        //暂停音效
    QSoundEffect *roate;            //旋转音效
    QSoundEffect *button;           //按钮音效


private:
    QList<int> rows;        //保存 已满行的y坐标
    int gamespeed;          //游戏速度
    QGraphicsScene *gameScene;



    //文字项
    QGraphicsSimpleTextItem * ScoreText;        //显示分数的文字提示
    QGraphicsSimpleTextItem * InitialRowText;   //显示初始行的文字提示
    QGraphicsSimpleTextItem * LevelText;        //显示等级与已消行的文字提示
    QGraphicsSimpleTextItem * InitialRow;       //初始行
    QGraphicsSimpleTextItem * Score;            //显示分数
    QGraphicsSimpleTextItem * Level;            //显示等级与已清除行数
    QGraphicsTextItem * Title;                  //标题文字
    QGraphicsTextItem * Over;                   //Over文字

    //音效开关 和 暂停开关 图标 添加动画
    QGraphicsPixmapItem *soundIcon;
    QGraphicsPixmapItem *pauseIcon;
    QTimer *timer;
    bool is_sound;


    void initMusic();
    void initinterface();
    void initbackbound();
    void initGamehint();
    void glint();


};

#endif // GAMEVIEW_H
