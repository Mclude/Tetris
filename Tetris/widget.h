#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QLable;
class QPushButton;
class QGraphicsScene;
class GameView;
class QGraphicsSimpleTextItem;
class QSoundEffect;
class QGraphicsPixmapItem;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
//    void keyPressEvent(QKeyEvent *event)override;



public:
    void initGameBtn();     //初始化游戏按钮
    //绘画相关
    void drawKeyShow(QPainter *painter);
    void deawGameRect(QPainter *painter);
    void drawbtnText(QPainter *painter);
    void drawbtnTextArrowKey(QPainter *painter);
    void drawbox(QPainter *painter);

private:
    Ui::Widget *ui;
    QPoint z;
    QColor Backcolor;

    //场景 视口
    GameView *gameView;


};
#endif // WIDGET_H
