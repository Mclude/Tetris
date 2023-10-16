#ifndef GAMEBOX_H
#define GAMEBOX_H

#include <QGraphicsItem>
#include <QPainter>

#include <QPropertyAnimation>

class GameBox : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ Color WRITE setColor)

public:
    //构造函数
    explicit GameBox(QGraphicsItem *parent = nullptr);

    //边框矩形
    QRectF boundingRect() const
    {
        qreal penWidth = 1;
        return QRectF(-m_radius - penWidth / 2, -m_radius - penWidth / 2,
                      m_radius*2 + penWidth, m_radius*2 + penWidth);
    }

    //实际图形
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setPen(QPen(m_color,3));
        painter->drawRect(this->boundingRect());
        painter->fillRect(QRect(-(m_radius*2-3)/2,-(m_radius*2-3)/2,m_radius*2-5,m_radius*2-5),m_color);

    }

    //实际形状
    QPainterPath shape() const
    {
        qreal real = m_radius-0.5;
        QPainterPath path;
        path.addRect(QRectF(-real,-real,real*2,real*2));
        return path;
    }

    //获取半径
    inline qreal getRadius()const{return m_radius;};

    //颜色属性使用的 读写函数
    QColor Color() const;
    void setColor(const QColor &newColor);

    void setChangeColorAnimated();  //设置颜色变化的动画
    void setAlphaColorAnimated();   //设置透明度变化的动画

    QPoint moveby() const;
    void setMoveby(QPoint newMoveby);

public:
    QPropertyAnimation *ChangeColor;    //保存颜色变化动画对象
    QPropertyAnimation *AlphaColor;     //保存透明度变化动画对象


private:
    qreal m_radius;     //方块半径
    QColor m_color;     //颜色属性



};
#endif // GAMEBOX_H

//
// #################### GameGroup ###################
//
#include <QGraphicsItemGroup>
#include <QTimer>

class QParallelAnimationGroup;

class GameGroup :public QObject ,public QGraphicsItemGroup
{
    Q_OBJECT
public:

    enum Shape{I,J,Z,L,O,S,T,Random};

    explicit GameGroup();

    QRectF boundingRect() const
    {
        qreal penWidth = 1;
        return QRectF(-m_diameter*2+6 - penWidth / 2, -m_diameter*2+6 - penWidth / 2,
                      m_diameter*4+6 + penWidth, m_diameter*4+6 + penWidth);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        Q_UNUSED(painter);

    }

signals:
    void needBox();
    void gameover();


public:

    void curateShape(const QPointF &point,Shape shape = Random);  //创建方块组
    QPoint ShapeCente();    //获取方块中心
    inline Shape getshape() const{return m_shape;}; //获取形状

    //游戏相关
    bool iscollsion();
    bool clearBoxGroup(bool is_delet);
    void statrTimer(int interval);
    void stopTimer();
    void movebyone();
    void myRotate(bool direction);

    //触发信号接口
    inline void signal_need(){ emit needBox();};
    inline void signal_over(){ emit gameover();};

    //开启动画
    void starAnimat();

private:

    Shape m_shape;  //形状
    QTimer *T_start;    //定时器
    qreal m_diameter = 14+6;  //方块直径
//    QTransform oldTransform;    //方块矩阵
    QParallelAnimationGroup * ParallelAnimat; //动画
    QList<GameBox *> shapegroup;    //方块列表

};
