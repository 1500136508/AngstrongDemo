#ifndef MYGRAPHICCALIPERITEM_H
#define MYGRAPHICCALIPERITEM_H
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
/* 缩放方向 */
enum EmDirection
{
	DIR_TOP = 0,
	DIR_BOTTOM,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_LEFTTOP,
	DIR_LEFTBOTTOM,
	DIR_RIGHTTOP,
	DIR_RIGHTBOTTOM,
	DIR_MIDDLE,
	DIR_NONE
};

#define EDGPADDING       5        //四周边缘可拉伸宽度
#define CORPADDING       6        //四角可拉伸宽度

#define MIN_WIDTH        5        //可拉伸的最小宽度
#define MIN_HEIGHT       5        //可拉伸的最小高度

#define POINT_WIDTH      6        //边缘9点的宽度
#define POINT_HEIGHT     6        //边缘9点的高度

#define EDGE_WIDTH       3        //边框的宽度
#define MIDDLELINE_WIDTH 2        //辅助线的宽度

#define DRAW_TEN_POINT            //绘制十个点
#define DRAW_SUB_LINE             //绘制辅助线

class QMenu;
class QAction;
class GraphicsRectItem :public QObject, public QGraphicsRectItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	GraphicsRectItem(QGraphicsRectItem *parent = nullptr);
	~GraphicsRectItem();
	QRect getRoiRect() const;               //获取已经圈选的框 外部调用
	void setBackImage(const QImage &img);   //设置背景图片  外部调用
	void setROIRect(QRect rect);
	void setTitle(QString qstrTitle);
protected:
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

	bool sceneEvent(QEvent *event);
private:
	void initViewer();                         //初始化
	void saveROIImage();                       //把ROI区域的图片存储下来
	EmDirection region(QPoint point);   //根据鼠标位置设置鼠标形状
	void scaleRect(const QPoint &mousePoint);  //缩放矩形
	void paintRect(const QPoint &mousePoint);  //绘制矩形
	void moveRect(const QPoint &mousePoint);   //移动矩形

	bool m_bPainterPressed;        //是否正在绘制
	bool m_bMovedPressed;          //是否正在拖动
	bool m_bScalePressed;          //是否正在缩放大小
	QPoint m_paintStartPoint;      //绘制的初始位置
	QPoint m_moveStartPoint;       //拖动的初始位置
	QRect m_roiRect;               //绘制的ROI
	EmDirection m_emCurDir;        //拖动的方向

	QImage m_backImage;            //背景图
	QString m_qstrTitle;           //标题

	QMenu *m_pOptMenu;
	QAction *m_pDelAction;
	QAction *m_pSaveAction;
};

#endif // MYGRAPHICCALIPERITEM_H
