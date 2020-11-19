#ifndef MYGRAPHICCALIPERITEM_H
#define MYGRAPHICCALIPERITEM_H
#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QRect>
#include <QPainter>
#include <QPolygon>
#include <QList>
#include <QTransform>
#include <QMap>
enum CALIPER_STATE_FLAG {
	CALI_DEFAULT = 0,
	CALI_RECT_TL,//标记当前为用户按下矩形的左上角
	CALI_RECT_TR,//标记当前为用户按下矩形的右上角
	CALI_RECT_BL,//左下角
	CALI_RECT_BR,//右下角
	CALI_RECT_SHEAR,//标记当前为用户按下矩形的下边中点的切变矩形
	CALI_RECT_MOVE,//标记当前为鼠标拖动矩形移动状态
	CALI_RECT_ROTATE,//标记当前为旋转状态
	CALIMOV_POLYGON_POINT//移动多边形的某个点
};
enum CALIPER_SHAPE_TYPE {
	CALIPER_RECT,//矩形
	CALIPER_LINE,
	CALIPER_CIRCLE,
	CALIPER_ELLIPSE
};

class GraphicsRectItem :public QObject, public QGraphicsItem
{
	Q_OBJECT
public:
	CALIPER_SHAPE_TYPE m_ShapeType;
	GraphicsRectItem(QGraphicsItem *parent = nullptr);
	~GraphicsRectItem();
	//GraphicsRectItem(QRectF m_OriginRect = QRectF(0,0,100,100));
	QRectF          boundingRect() const;
	QPainterPath    shape() const;
	QPainterPath    getCollideShape();
	QPainterPath    getCollideShapeTopLeftAsOriginPoint();
	void setRectSize(QRectF mrect, bool bResetRotateCenter = true);
	void setShearRectSize(QRectF mrect);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	int type() const;
	void SetRotate(qreal RotateAngle);
	void SetSideCursorByRotateAngle(qreal RotateAngle);
	//计算两点距离
	double CalDis(const double x1, const double y1, const double x2, const double y2);
	//计算点(ptx,pty)到线段(x1y1,x2y2)的距离
	double CalDis(const double ptx, const double pty, const double x1, const double y1, const double x2, const double y2);
	double CalDisPoint2longLine(const double ptx, const double pty, const double x1, const double y1, const double x2, const double y2);
	//计算斜切后分别经过中心点和鼠标所在点的两平行线的2个焦点
	bool get2CrossPt(QPointF p0, QPointF p1, QPointF p2, QPointF pc, QPointF pos, QPointF &pCross1, QPointF &pCross2);
	QPointF pCross1, pCross2, pCross3, pCross5;
	QPointF         getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle);//获取旋转后的点
	QList<QPointF>  getRotatePoints(QPointF ptCenter, QList<QPointF> ptIns, qreal angle);//获取多个旋转后的点
	QPolygonF       getRotatePolygonFromRect(QPointF ptCenter, QRectF rectIn, qreal angle);//将矩形旋转之后返回多边形
	QPolygonF       getRotatePolygonFromPolygon(QPointF ptCenter, QPolygonF polyIn, qreal angle);//将多边形旋转之后返回多边形
	QRectF          getBoundedRectToSceen();
	QPolygonF       getCrtPolygonToScreen();
	QPointF getSmallRotateRectCenter(QPointF ptA, QPointF ptB);//获取旋转时候矩形正上方的旋转标记矩形
	QRectF  getSmallRotateRect(QPointF ptA, QPointF ptB);
	qreal   m_RotateAngle;
	QPointF m_RotateCenter;
	bool        m_bKeepShadowLength;//保持投影长度不变
private:
	//CALIPER_RECT矩形卡尺使用
	QRectF      m_RECT;
	QRectF      m_newShearRECT;
	QPolygonF   m_RECTShear;//矩形斜切后
	QPolygonF   m_RECT_Pol;//矩形旋转后
	QPolygonF   m_RECT_TL_Pol;//左上角顶点旋转后
	QRectF      m_RECT_TL;//左上角顶点
	QPolygonF   m_RECT_TR_Pol;//右上角顶点旋转后
	QRectF      m_RECT_TR;//右上角顶点
	QPolygonF   m_RECT_BL_Pol;//左下角顶点旋转后
	QRectF      m_RECT_BL;//左下角顶点
	QPolygonF   m_RECT_BR_Pol;//右下角顶点旋转后
	QRectF      m_RECT_BR;//右下角顶点
	QPolygonF   m_RECT_Inside_Pol;//内部区域旋转后
	QPolygonF   m_RECT_InsideShear;//内部区域斜切后
	QRectF      m_RECT_Inside;//内部区域
	QPolygonF   m_RECT_Scan_Dir_Pol;//扫描方向标记旋转后
	QRectF      m_RECT_Scan_Dir;//扫描方向标记
	QPolygonF   m_RECT_Shadow_Dir_Pol;//投影方向标记旋转后
	QRectF      m_RECT_Shadow_Dir;//投影方向标记
	qreal       m_fShadowLength;//投影长度
	QPolygonF   m_RECT_Rotate_Pol;//旋转的标记的矩形旋转后形成的多边形
	QRectF      m_RECT_Rotate;//旋转的标记的矩形
	QPolygonF   m_RECT_Shear_Pol;//切变矩形标记旋转后
	QRectF      m_RECT_Shear;//切变矩形标记
	qreal       m_fShearX;//切变矩形x方向参数

	//
	QPointF     m_startPos;
	CALIPER_STATE_FLAG  m_StateFlag;
	QMenu       *pMenu;//弹出菜单
	QPolygonF   m_oldPolygon;
	QMap<qreal, int> m_MapDis2Line;//记录鼠标右击时的坐标离m_oldPolygon中每一条线段的距离
	qreal       m_MinDis;//记录鼠标右击时，离m_oldPolygon最近一条线段的距离；
	int         m_nPolygonMovePointIndex;//移动多边形顶点的索引
	int         m_nPolyRemoveIndex;

	bool        m_bResize;
protected:
private slots:
	void onMenuEvent();//弹出菜单点击后响应函数
};

#endif // MYGRAPHICCALIPERITEM_H
