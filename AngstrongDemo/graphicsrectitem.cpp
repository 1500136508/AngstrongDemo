#include "graphicsrectitem.h"
#include <QtMath>
#include <QDebug>
#include <QMenu>

#pragma execution_character_set("utf-8")//让能够正常显示中文字符串

GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent) :
	m_ShapeType(CALIPER_RECT),
	m_bResize(false),
	m_RECT(-100, -200, 200, 400),
	m_RotateAngle(0),
	m_bKeepShadowLength(false),
	m_fShearX(0),
	m_StateFlag(CALI_DEFAULT)
{
	//setParent(parent);
	m_newShearRECT.setWidth(0);
	setRectSize(m_RECT);
	//setToolTip("Click and drag me!");  //提示
	setCursor(Qt::ArrowCursor);   //改变光标形状,手的形状
	setFlag(QGraphicsItem::ItemIsMovable);
	//SetRotate(0);
	//setFlag(QGraphicsItem::ItemIsSelectable);//
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	//    //创建菜单对象
	//    pMenu = new QMenu();//((QWidget*)this);
	//    QAction *pActionAddPt = new QAction("添加顶点", pMenu);
	//    QAction *pActionDelPt = new QAction("删除顶点", pMenu);
	//    //1:添加多边形顶点 2:删除多边形顶点
	//    pActionAddPt->setData(1);
	//    pActionDelPt->setData(2);

	//    //把QAction对象添加到菜单上
	//    pMenu->addAction(pActionAddPt);
	//    pMenu->addAction(pActionDelPt);

	//    //连接鼠标右键点击信号
	//    connect(pActionAddPt, SIGNAL(triggered()), this, SLOT(onMenuEvent()));
	//    connect(pActionDelPt, SIGNAL(triggered()), this, SLOT(onMenuEvent()));
}
void GraphicsRectItem::onMenuEvent()
{
	//    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
	//    //获取发送信息类型 //1:添加多边形顶点 2:删除多边形顶点
	//    int iType = pEven->data().toInt();
	//    switch (iType)
	//    {
	//        case 1://添加多边形顶点
	//            m_oldPolygon.insert(m_MapDis2Line.value(m_MinDis)+1,m_startPos);
	//        break;
	//        case 2://删除多边形顶点
	//            if(m_oldPolygon.count()>3)
	//            {
	//                m_oldPolygon.remove(m_nPolyRemoveIndex);
	//            }
	//            break;
	//        default:
	//            break;
	//    }
	//    scene()->update();
}
QRectF GraphicsRectItem::boundingRect() const//用来控制本item绘制区域
{
	QPainterPath path;
	if (m_ShapeType == CALIPER_RECT)
	{
		path.setFillRule(Qt::WindingFill);
		path.addPolygon(m_RECT_Pol);
		path.addPolygon(m_RECT_Rotate_Pol);
		path.addPolygon(m_RECT_TL_Pol);
		path.addPolygon(m_RECT_TR_Pol);
		path.addPolygon(m_RECT_BL_Pol);
		path.addPolygon(m_RECT_BR_Pol);
		path.addPolygon(m_RECT_Shadow_Dir_Pol);
		path.addPolygon(m_RECT_Shear_Pol);
	}
	return path.boundingRect();
}

QPainterPath GraphicsRectItem::shape() const//用来控制检测碰撞collide和鼠标点击hit响应区域
{
	QPainterPath path;
	if (m_ShapeType == CALIPER_RECT)
	{
		path.setFillRule(Qt::WindingFill);
		path.addPolygon(m_RECT_Pol);
		path.addPolygon(m_RECT_Rotate_Pol);
		path.addPolygon(m_RECT_TL_Pol);
		path.addPolygon(m_RECT_TR_Pol);
		path.addPolygon(m_RECT_BL_Pol);
		path.addPolygon(m_RECT_BR_Pol);
		path.addPolygon(m_RECT_Shadow_Dir_Pol);
		path.addPolygon(m_RECT_Shear_Pol);
	}
	return path;
}

QPainterPath GraphicsRectItem::getCollideShape()
{
	QPainterPath path;
	if (m_ShapeType == CALIPER_RECT)
	{
		path.addPolygon(m_RECT_Pol);
	}
	return path;
}

QPainterPath GraphicsRectItem::getCollideShapeTopLeftAsOriginPoint()//返回左上角移动到item的（0,0）位置的collide区域
{
	QPainterPath path;
	if (m_ShapeType == CALIPER_RECT)
	{
		QTransform trans;
		path.addPolygon(m_RECT_Pol);
		trans.translate(-path.boundingRect().x(), -path.boundingRect().y());
		path = trans.map(path);
	}
	return path;
}

GraphicsRectItem::~GraphicsRectItem()
{

}

void GraphicsRectItem::setRectSize(QRectF mrect, bool bResetRotateCenter)
{
	m_RECT = mrect;
	//    if(m_newShearRECT.width() == 0)
	//    {
	m_newShearRECT = m_RECT;
	//    }
	if (bResetRotateCenter)
	{
		m_RotateCenter.setX(m_RECT.x() + m_RECT.width() / 2);
		m_RotateCenter.setY(m_RECT.y() + m_RECT.height() / 2);
	}
	//test 扭曲
	m_RECTShear = QPolygonF(m_RECT);
	QTransform trans;
	trans.translate(m_RECT.center().x(), m_RECT.center().y());
	trans.shear(m_fShearX, 0);
	trans.translate(-m_RECT.center().x(), -m_RECT.center().y());
	m_RECTShear = trans.map(m_RECTShear);//斜切之后的矩形
	//test
	m_RECT_Pol = getRotatePolygonFromPolygon(m_RotateCenter, m_RECTShear, m_RotateAngle);

	//m_RECT_Inside = QRectF(m_RECTShear[0].x()+5,m_RECTShear[0].y()+5,m_RECT.width()-10,m_RECT.height()-10);
	m_RECT_InsideShear.clear();
	m_RECT_InsideShear.append(QPointF(m_RECTShear[0].x() + 5, m_RECTShear[0].y() + 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[1].x() - 5, m_RECTShear[1].y() + 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[2].x() - 5, m_RECTShear[2].y() - 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[3].x() + 5, m_RECTShear[3].y() - 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[0].x() + 5, m_RECTShear[0].y() + 5));
	m_RECT_Inside_Pol = getRotatePolygonFromPolygon(m_RotateCenter, m_RECT_InsideShear, m_RotateAngle);

	m_RECT_TL = QRectF(m_RECTShear[0].x() - 5, m_RECTShear[0].y() - 5, 10, 10);//左上角顶点
	m_RECT_TL_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_TL, m_RotateAngle);

	m_RECT_TR = QRectF(m_RECTShear[1].x() - 5, m_RECTShear[1].y() - 5, 10, 10);//右上角顶点
	m_RECT_TR_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_TR, m_RotateAngle);

	m_RECT_BL = QRectF(m_RECTShear[3].x() - 5, m_RECTShear[3].y() - 5, 10, 10);//左下角顶点
	m_RECT_BL_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_BL, m_RotateAngle);

	m_RECT_BR = QRectF(m_RECTShear[2].x() - 5, m_RECTShear[2].y() - 5, 10, 10);//右下角顶点
	m_RECT_BR_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_BR, m_RotateAngle);

	m_RECT_Rotate = QRectF((m_RECTShear[1].x() + m_RECTShear[2].x()) / 2 - 10, (m_RECTShear[1].y() + m_RECTShear[2].y()) / 2 - 10, 20, 20);//矩形右侧旋转标记矩形
	m_RECT_Rotate_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_Rotate, m_RotateAngle);

	m_RECT_Scan_Dir = QRectF((m_RECTShear[0].x() + m_RECTShear[1].x()) / 2 - 5, (m_RECTShear[0].y() + m_RECTShear[1].y()) / 2 - 5, 10, 10);
	QPolygonF tempPol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_Scan_Dir, m_RotateAngle);
	QVector<QPointF> tempv;
	tempv.append(QPointF(tempPol[0]));
	tempv.append(QPointF((tempPol[1].x() + tempPol[2].x()) / 2, (tempPol[1].y() + tempPol[2].y()) / 2));
	tempv.append(QPointF(tempPol[3]));
	m_RECT_Scan_Dir_Pol = QPolygonF(tempv);//扫描方向的三角形

	m_RECT_Shadow_Dir = QRectF(m_RECT.x() - 5, m_RECT.bottomLeft().y() - 15, 10, 10);//投影方向的三角形标记
	QPolygonF tempShadow = QPolygonF(m_RECT_Shadow_Dir);
	tempShadow = trans.map(tempShadow);
	m_RECT_Shadow_Dir_Pol = getRotatePolygonFromPolygon(m_RotateCenter, tempShadow, m_RotateAngle);

	m_RECT_Shear = QRectF((m_RECTShear[2].x() + m_RECTShear[3].x()) / 2 - 5, (m_RECTShear[2].y() + m_RECTShear[3].y()) / 2 - 5, 10, 10);
	//    tempPol = getRotatePolygonFromRect(m_RotateCenter,m_RECT_Shear,m_RotateAngle);
	//    tempv.clear();
	//    tempv.append(QPointF((tempPol[0].x()+tempPol[1].x())/2,(tempPol[0].y()+tempPol[1].y())/2));
	//    tempv.append(tempPol[1]);
	//    tempv.append(QPointF((tempPol[2].x()+tempPol[3].x())/2,(tempPol[2].y()+tempPol[3].y())/2));
	//    tempv.append(tempPol[3]);
	//    m_RECT_Shear_Pol = QPolygonF(tempv);//切变矩形标记
	QPolygonF tempShear = QPolygonF(m_RECT_Shear);
	QTransform transShear;
	transShear.translate(m_RECT_Shear.center().x(), m_RECT_Shear.center().y());//先把中心移动到m_RECT_Shear中心
	transShear.shear(1, 0);
	transShear.translate(-m_RECT_Shear.center().x(), -m_RECT_Shear.center().y());//先把中心移动到m_RECT_Shear中心
	tempShear = transShear.map(tempShear);//斜切之后的矩形
	m_RECT_Shear_Pol = getRotatePolygonFromPolygon(m_RotateCenter, tempShear, m_RotateAngle);

}

void GraphicsRectItem::setShearRectSize(QRectF mrect)
{
	m_newShearRECT = mrect;
	m_RotateCenter.setX(m_newShearRECT.x() + m_newShearRECT.width() / 2);
	m_RotateCenter.setY(m_newShearRECT.y() + m_newShearRECT.height() / 2);
	//test 扭曲
	m_RECTShear = QPolygonF(m_newShearRECT);
	QTransform trans;
	trans.translate(m_newShearRECT.center().x(), m_newShearRECT.center().y());
	trans.shear(m_fShearX, 0);
	trans.translate(-m_newShearRECT.center().x(), -m_newShearRECT.center().y());
	m_RECTShear = trans.map(m_RECTShear);//斜切之后的矩形
	//test
	m_RECT_Pol = getRotatePolygonFromPolygon(m_RotateCenter, m_RECTShear, m_RotateAngle);

	//m_RECT_Inside = QRectF(m_RECTShear[0].x()+5,m_RECTShear[0].y()+5,m_RECT.width()-10,m_RECT.height()-10);
	m_RECT_InsideShear.clear();
	m_RECT_InsideShear.append(QPointF(m_RECTShear[0].x() + 5, m_RECTShear[0].y() + 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[1].x() - 5, m_RECTShear[1].y() + 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[2].x() - 5, m_RECTShear[2].y() - 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[3].x() + 5, m_RECTShear[3].y() - 5));
	m_RECT_InsideShear.append(QPointF(m_RECTShear[0].x() + 5, m_RECTShear[0].y() + 5));
	m_RECT_Inside_Pol = getRotatePolygonFromPolygon(m_RotateCenter, m_RECT_InsideShear, m_RotateAngle);

	m_RECT_TL = QRectF(m_RECTShear[0].x() - 5, m_RECTShear[0].y() - 5, 10, 10);//左上角顶点
	m_RECT_TL_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_TL, m_RotateAngle);

	m_RECT_TR = QRectF(m_RECTShear[1].x() - 5, m_RECTShear[1].y() - 5, 10, 10);//右上角顶点
	m_RECT_TR_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_TR, m_RotateAngle);

	m_RECT_BL = QRectF(m_RECTShear[3].x() - 5, m_RECTShear[3].y() - 5, 10, 10);//左下角顶点
	m_RECT_BL_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_BL, m_RotateAngle);

	m_RECT_BR = QRectF(m_RECTShear[2].x() - 5, m_RECTShear[2].y() - 5, 10, 10);//右下角顶点
	m_RECT_BR_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_BR, m_RotateAngle);

	m_RECT_Rotate = QRectF((m_RECTShear[1].x() + m_RECTShear[2].x()) / 2 - 10, (m_RECTShear[1].y() + m_RECTShear[2].y()) / 2 - 10, 20, 20);//矩形右侧旋转标记矩形
	m_RECT_Rotate_Pol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_Rotate, m_RotateAngle);

	m_RECT_Scan_Dir = QRectF((m_RECTShear[0].x() + m_RECTShear[1].x()) / 2 - 5, (m_RECTShear[0].y() + m_RECTShear[1].y()) / 2 - 5, 10, 10);
	QPolygonF tempPol = getRotatePolygonFromRect(m_RotateCenter, m_RECT_Scan_Dir, m_RotateAngle);
	QVector<QPointF> tempv;
	tempv.append(QPointF(tempPol[0]));
	tempv.append(QPointF((tempPol[1].x() + tempPol[2].x()) / 2, (tempPol[1].y() + tempPol[2].y()) / 2));
	tempv.append(QPointF(tempPol[3]));
	m_RECT_Scan_Dir_Pol = QPolygonF(tempv);//扫描方向的三角形

	m_RECT_Shadow_Dir = QRectF(m_newShearRECT.x() - 5, m_newShearRECT.bottomLeft().y() - 15, 10, 10);//投影方向的三角形标记
	QPolygonF tempShadow = QPolygonF(m_RECT_Shadow_Dir);
	tempShadow = trans.map(tempShadow);
	m_RECT_Shadow_Dir_Pol = getRotatePolygonFromPolygon(m_RotateCenter, tempShadow, m_RotateAngle);

	m_RECT_Shear = QRectF((m_RECTShear[2].x() + m_RECTShear[3].x()) / 2 - 5, (m_RECTShear[2].y() + m_RECTShear[3].y()) / 2 - 5, 10, 10);
	QPolygonF tempShear = QPolygonF(m_RECT_Shear);
	QTransform transShear;
	transShear.translate(m_RECT_Shear.center().x(), m_RECT_Shear.center().y());//先把中心移动到m_RECT_Shear中心
	transShear.shear(1, 0);
	transShear.translate(-m_RECT_Shear.center().x(), -m_RECT_Shear.center().y());//先把中心移动到m_RECT_Shear中心
	tempShear = transShear.map(tempShear);//斜切之后的矩形
	m_RECT_Shear_Pol = getRotatePolygonFromPolygon(m_RotateCenter, tempShear, m_RotateAngle);

}

void GraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen mPen;
	mPen = QPen(Qt::yellow);
	painter->setPen(mPen);
	if (m_ShapeType == CALIPER_RECT)
	{
		//绘制旋转后的矩形
		painter->drawPolygon(m_RECT_Pol);
		//绘制旋转圆形
		mPen.setWidth(2);
		mPen.setColor(Qt::green);
		painter->setPen(mPen);
		QPointF pf = QPointF((m_RECT_Pol[1].x() + m_RECT_Pol[2].x()) / 2, (m_RECT_Pol[1].y() + m_RECT_Pol[2].y()) / 2);
		QRectF rect = QRectF(pf.x() - 10, pf.y() - 10, 20, 20);
		painter->drawEllipse(rect);//绘制圆形
		painter->drawPoint(pf);//绘制点
		//绘制4个顶点矩形
		mPen.setWidth(1);
		painter->setPen(mPen);
		//        painter->drawPolygon(m_RECT_TL_Pol);
		//        painter->drawPolygon(m_RECT_TR_Pol);
		//        painter->drawPolygon(m_RECT_BL_Pol);
		painter->drawPolygon(m_RECT_BR_Pol);
		//绘制投影标记
		QPointF pt0 = m_RECT_Shadow_Dir_Pol[0];
		QPointF pt1 = QPointF((m_RECT_Shadow_Dir_Pol[2].x() + m_RECT_Shadow_Dir_Pol[3].x()) / 2, (m_RECT_Shadow_Dir_Pol[2].y() + m_RECT_Shadow_Dir_Pol[3].y()) / 2);
		QPointF pt2 = m_RECT_Shadow_Dir_Pol[1];
		painter->drawLine(pt0, pt1);
		painter->drawLine(pt2, pt1);
		//绘制扫描方向标记
		painter->drawPolygon(m_RECT_Scan_Dir_Pol);
		//绘制切变矩形标记
		painter->drawPolygon(m_RECT_Shear_Pol);

		//painter->drawPolygon(m_RECT_Inside_Pol);
//        mPen.setColor(Qt::red);
//        mPen.setWidth(5);
//        painter->setPen(mPen);
//        painter->drawPoint(pCross1);
//        painter->drawPoint(pCross2);
//        painter->drawPoint(pCross3);
//        painter->drawPoint(m_RECT.center());
	}
}

double GraphicsRectItem::CalDis(const double x1, const double y1, const double x2, const double y2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

double GraphicsRectItem::CalDis(const double ptx, const double pty, const double x1, const double y1, const double x2, const double y2)
{
	double f = (x2 - x1)*(ptx - x1) + (y2 - y1)*(pty - y1);
	if (f <= 0)
		return CalDis(ptx, pty, x1, y1);
	double d = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
	if (f > d)
		return CalDis(ptx, pty, x2, y2);
	f = f / d;
	double projX = (x1 + (x2 - x1)*f);
	double projY = (y1 + (y2 - y1)*f);
	return CalDis(ptx, pty, projX, projY);
}

double GraphicsRectItem::CalDisPoint2longLine(const double ptx, const double pty, const double x1, const double y1, const double x2, const double y2)
{
	double f = (x2-x1)*(ptx-x1) + (y2-y1)*(pty-y1);
	if (f <= 0)
		return CalDis(ptx, pty, x1, y1);
	double d = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
	if (f > d)
		return CalDis(ptx, pty, x2, y2);
	//    f = f/d;
	//    double projX = (x1 + (x2 - x1)*f);
	//    double projY = (y1 + (y2 - y1)*f);
	//    return CalDis(ptx, pty, projX, projY);

		//求点(ptx,pty)到直线(x1,y1)(x2,y2)的距离
		//直线(x1,y1)(x2,y2)的方程为y=kx+b;
		//过点(ptx,pty)的直线(x1,y1)(x2,y2)的中垂线方程为y=k2x+b2;两线交点为pCross(x0,y0)
	qreal k, b, k2, b2, x0, y0;
	if (x1 == x2)
	{
		//直线方程为：x=x1
		if (y1 == y2)
		{
			//垂线方程为：x=ptx
			//该情况异常，返回-1
			return -1;
		}
		else
		{
			k2 = (x2 - x1) / (y1 - y2);
			b2 = pty - k2 * ptx;
			x0 = x1;
			y0 = k2 * x0 + b2;
		}
	}
	else
	{
		k = (y1 - y2) / (x1 - x2);
		b = y1 - k * x1;
		if (y1 == y2)
		{
			//垂线方程为：x=ptx
			x0 = ptx;
			y0 = k * x0 + b;
		}
		else
		{
			k2 = (x2 - x1) / (y1 - y2);
			b2 = pty - k2 * ptx;
			if (k == k2)
				return -1;
			else
			{
				x0 = (b - b2) / (k2 - k);
				y0 = k * x0 + b;
			}
		}
	}
	return CalDis(x0, y0, ptx, pty);
}

bool GraphicsRectItem::get2CrossPt(QPointF p0, QPointF p1, QPointF p2, QPointF pc, QPointF pos, QPointF &pCross1, QPointF &pCross2)
{
	qreal k1, k2, b1, b2;
	//假设经过pc的平行于p0p1的直线方程为：y=k1*x+b1;
	if (p1.x() != p0.x())
	{
		k1 = (p1.y() - p0.y()) / (p1.x() - p0.x());
		b1 = pc.y() - k1 * pc.x();
	}
	//假设经过pos的平行于p1p2的直线方程为：y=k2*x+b2;
	if (p1.x() != p2.x())
	{
		k2 = (p2.y() - p1.y()) / (p2.x() - p1.x());
		b2 = pos.y() - k2 * pos.x();
	}
	//假设两线交点为(x,y)
	qreal x, y;
	if (p1.x() == p0.x())
	{
		//则k1不存在,直线1为x=pc.x()
		if (p1.x() == p2.x())
		{
			//k2不存在，直线2为x=pos.x()
			//则两线焦点不存在，直接return
			return false;
		}
		else
		{
			x = pc.x();
			y = k2 * x + b2;
		}
	}
	else
	{
		if (p1.x() == p2.x())
		{
			//k2不存在，直线2为x=pos.x()
			x = pos.x();
			y = k1 * x + b1;
		}
		else
		{
			if (k1 == k2)
				return false;
			x = (b1 - b2) / (k2 - k1);
			y = k1 * x + b1;
		}
	}
	pCross1 = QPointF(x, y);//经过pc的平行于p0p1的直线 与 经过Pos的平行于p1p2的直线 的焦点

	//假设经过pos的平行于p0p1的直线方程为：y=k3*x+b3;
	//假设两线交点为(x2,y2)
	qreal k3, k4, b3, b4, x2, y2;
	if (p1.x() != p0.x())
	{
		k3 = (p1.y() - p0.y()) / (p1.x() - p0.x());
		b3 = pos.y() - k3 * pos.x();
	}
	//假设经过pc的平行于p1p2的直线方程为：y=k4*x+b4;
	if (p1.x() != p2.x())
	{
		k4 = (p2.y() - p1.y()) / (p2.x() - p1.x());
		b4 = pc.y() - k4 * pc.x();
	}
	if (p1.x() == p0.x())
	{
		//则k3不存在,直线1为x=pos.x()
		if (p1.x() == p2.x())
		{
			//k4不存在，直线2为x=pc.x()
			//则两线焦点不存在，直接return
			return false;
		}
		else
		{
			x2 = pos.x();
			y2 = k4 * x2 + b4;
		}
	}
	else
	{
		if (p1.x() == p2.x())
		{
			//k4不存在，直线2为x=pc.x()
			x2 = pc.x();
			y2 = k3 * x2 + b3;
		}
		else
		{
			if (k3 == k4)
				return false;
			x2 = (b3 - b4) / (k4 - k3);
			y2 = k3 * x2 + b3;
		}
	}
	pCross2 = QPointF(x2, y2);//经过pc的平行于p0p1的直线 与 经过Pos的平行于p1p2的直线 的焦点
	return true;
}

void GraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		//setSelected(true);
		m_startPos = event->pos();//鼠标左击时，获取当前鼠标在图片中的坐标，
		if (m_ShapeType == CALIPER_RECT)//矩形卡尺
		{
			if (m_RECT_Rotate_Pol.containsPoint(m_startPos, Qt::WindingFill))//旋转矩形
			{
				m_StateFlag = CALIPER_STATE_FLAG::CALI_RECT_ROTATE;
				setCursor(Qt::PointingHandCursor);
			}
			//            else if(m_RECT_TL_Pol.containsPoint(m_startPos,Qt::WindingFill))
			//            {
			//                m_StateFlag = CALI_RECT_TL;//标记当前为用户按下矩形的左上角顶点矩形区域
			//                setCursor(Qt::SizeAllCursor);
			//                //SetSideCursorByRotateAngle(m_RotateAngle);
			//            }
			//            else if(m_RECT_TR_Pol.containsPoint(m_startPos,Qt::WindingFill))
			//            {
			//                m_StateFlag = CALI_RECT_TR;//标记当前为用户按下矩形的右上角顶点矩形区域
			//                setCursor(Qt::SizeAllCursor);
			//                //SetSideCursorByRotateAngle(m_RotateAngle);
			//            }
			else if (m_RECT_BR_Pol.containsPoint(m_startPos, Qt::WindingFill))
			{
				m_StateFlag = CALI_RECT_BR;//标记当前为用户按下矩形的右下角顶点矩形区域
				setCursor(Qt::SizeAllCursor);
				//SetSideCursorByRotateAngle(m_RotateAngle);
			}
			//            else if(m_RECT_BL_Pol.containsPoint(m_startPos,Qt::WindingFill))
			//            {
			//                m_StateFlag = CALI_RECT_BL;//标记当前为用户按下矩形的左下角顶点矩形区域
			//                setCursor(Qt::SizeAllCursor);
			//                //SetSideCursorByRotateAngle(m_RotateAngle);
			//            }
			else if (m_RECT_Shear_Pol.containsPoint(m_startPos, Qt::WindingFill))//在矩形内框区域时按下鼠标，则可拖动图片
			{
				m_StateFlag = CALIPER_STATE_FLAG::CALI_RECT_SHEAR;//标记当前为鼠标拖动图片移动状态
				setCursor(Qt::PointingHandCursor);   //改变光标形状,手指的形状
			}
			else if (m_RECT_Inside_Pol.containsPoint(m_startPos, Qt::WindingFill))//在矩形内框区域时按下鼠标，则可拖动图片
			{
				m_StateFlag = CALIPER_STATE_FLAG::CALI_RECT_MOVE;//标记当前为鼠标拖动图片移动状态
				setCursor(Qt::ClosedHandCursor);   //改变光标形状,手的形状
			}
			else
			{
				m_StateFlag = CALI_DEFAULT;
			}
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		m_startPos = event->pos();
		//pMenu->exec(event->screenPos());//弹出菜单
	}
	else
	{
		QGraphicsItem::mousePressEvent(event);
	}
}

void GraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_ShapeType == CALIPER_RECT)//矩形
	{
		if (m_StateFlag == CALI_RECT_ROTATE)
		{
			int nRotateAngle = atan2((event->pos().x() - m_RotateCenter.x()), (event->pos().y() - m_RotateCenter.y())) * 180 / M_PI;
			SetRotate(90 - nRotateAngle);
			//qDebug()<<nRotateAngle;
		}
		else if (m_StateFlag == CALI_RECT_MOVE)
		{
			QPointF point = (event->pos() - m_startPos);
			moveBy(point.x(), point.y());
			scene()->update();
		}
		else if (m_StateFlag == CALI_RECT_SHEAR)//控制矩形斜切
		{
			QPointF pc, p0, p1, p2, p3;//,pCross1,pCross2;
			pc = m_RotateCenter;
			p0 = m_RECT_Pol[0];
			p1 = m_RECT_Pol[1];
			p2 = m_RECT_Pol[2];
			p3 = m_RECT_Pol[3];
			if (!get2CrossPt(p0, p1, p2, pc, event->pos(), pCross1, pCross2))
				return;
			//过pc做p2p3的垂线line3,line3方程为y=k3x+b3,line3与p2,p3交点为pCross3
			qreal k3, b3, k4, b4;
			qreal x3, y3;//pCross3(x3,y3),//y3=k3x3+b3,y3=k4x3+b4,求x3,y3
			if (p3.y() == p2.y())
			{
				//k3不存在，lin3为x=pc.x()
				//直线p2p3为y=k4x+b4
				if (p3.x() == p2.x())
				{
					//k4不存在，直线p2p3为x=p2.x()
					return;
				}
				else
				{
					k4 = (p3.y() - p2.y()) / (p3.x() - p2.x());
					b4 = p2.y() - k4 * p2.x();
					//TODO:求(x3,y3)
					x3 = pc.x();
					y3 = k4 * x3 + b4;
				}
			}
			else
			{
				k3 = (p2.x() - p3.x()) / (p3.y() - p2.y());
				b3 = pc.y() - k3 * pc.x();
				//直线p2p3为y=k4x+b4
				if (p3.x() == p2.x())
				{
					//k4不存在，直线p2p3为x=p2.x()
					//TODO:求(x3,y3)
					x3 = p2.x();
					y3 = k3 * x3 + b3;
				}
				else
				{
					k4 = (p3.y() - p2.y()) / (p3.x() - p2.x());
					b4 = p2.y() - k4 * p2.x();
					//TODO:求(x3,y3)
					if (k3 != k4)
					{
						x3 = (b4 - b3) / (k3 - k4);
						y3 = k3 * x3 + b3;
					}
				}
			}
			pCross3 = QPointF(x3, y3);

			//过pos做p2p3的垂线line5,line5方程为y=k5x+b5,line5与p2,p3交点为pCross5
			qreal k5, b5;
			qreal x5, y5;//pCross5(x5,y5),//y5=k5x5+b5,y5=k4x5+b4,求x5,y5
			if (p3.y() == p2.y())
			{
				//k3不存在，lin5为x=event->pos().x()
				//直线p2p3为y=k4x+b4
				if (p3.x() == p2.x())
				{
					//k4不存在，直线p2p3为x=p2.x()
					return;
				}
				else
				{
					k4 = (p3.y() - p2.y()) / (p3.x() - p2.x());
					b4 = p2.y() - k4 * p2.x();
					//TODO:求(x5,y5)
					x5 = event->pos().x();
					y5 = k4 * x5 + b4;
				}
			}
			else
			{
				k5 = (p2.x() - p3.x()) / (p3.y() - p2.y());
				b5 = event->pos().y() - k5 * event->pos().x();
				//直线p2p3为y=k4x+b4
				if (p3.x() == p2.x())
				{
					//k4不存在，直线p2p3为x=p2.x()
					//TODO:求(x5,y5)
					x5 = p2.x();
					y5 = k5 * x5 + b5;
				}
				else
				{
					k4 = (p3.y() - p2.y()) / (p3.x() - p2.x());
					b4 = p2.y() - k4 * p2.x();
					//TODO:求(x5,y5)
					if (k5 != k4)
					{
						x5 = (b4 - b5) / (k5 - k4);
						y5 = k5 * x5 + b5;
					}
				}
			}
			pCross5 = QPointF(x5, y5);

			//求pos到垂线line3的距离为disShear
			double disShear = CalDisPoint2longLine(event->pos().x(), event->pos().y(), pc.x(), pc.y(), pCross3.x(), pCross3.y());
			//            double disp3pos = CalDis(p3.x(),p3.y(),event->pos().x(),event->pos().y());
			//            double disp2pos = CalDis(p2.x(),p2.y(),event->pos().x(),event->pos().y());
			//            double disp3pCross3 = CalDis(p3.x(),p3.y(),pCross3.x(),pCross3.y());
			//            double disp2pCross3 = CalDis(p2.x(),p2.y(),pCross3.x(),pCross3.y());
			//            double disVertical2RightLine = CalDisPoint2longLine(event->pos().x(),event->pos().y(),p1.x(),p1.y(),p2.x(),p2.y());
			//            double disVertical2LeftLine = CalDisPoint2longLine(event->pos().x(),event->pos().y(),p0.x(),p0.y(),p3.x(),p3.y());
			double newHalfHeight;
			if (m_bKeepShadowLength)
			{
				if (disShear > m_RECT.height() / 2 - 1)
					disShear = m_RECT.height() / 2 - 1;
				newHalfHeight = sqrt(qPow(m_RECT.height() / 2, 2) - disShear * disShear);
				m_fShearX = disShear / newHalfHeight;
			}
			else
			{
				m_fShearX = disShear / (m_RECT.height() / 2);
			}
			//确定往左斜切还是往右斜切
			if (((-90 < m_RotateAngle) && (m_RotateAngle < 90)) || (m_RotateAngle > 270 && m_RotateAngle < 360) || m_RotateAngle == 360)
			{
				if (pCross5.x() < pCross3.x())
					m_fShearX = -m_fShearX;
			}
			else if (m_RotateAngle == 90)
			{
				if (pCross5.y() < pCross3.y())
					m_fShearX = -m_fShearX;
			}
			else if (m_RotateAngle > 90 && m_RotateAngle < 270)
			{
				if (pCross5.x() > pCross3.x())
					m_fShearX = -m_fShearX;
			}
			else if (m_RotateAngle == 270 || m_RotateAngle == -90)
			{
				if (pCross5.y() > pCross3.y())
					m_fShearX = -m_fShearX;
			}
			if (m_bKeepShadowLength)
			{
				setShearRectSize(QRectF(pc.x() - m_RECT.width() / 2, pc.y() - newHalfHeight, m_newShearRECT.width(), newHalfHeight * 2));//这个函数不会重置原始矩形的大小
			}
			else
			{
				setRectSize(m_RECT);
			}
			scene()->update();
			//            //如果是要保持投影长度不变，则矩形需要重新及计算切变前矩形的高度
			//            if(m_bKeepShadowLength)
			//            {
			//                double newHalfHeight = sqrt(qPow(m_RECT.height()/2,2)/(m_fShearX*m_fShearX+1));
			//                setShearRectSize(QRectF(pc.x()-m_RECT.width()/2,pc.y()-newHalfHeight,m_RECT.width(),newHalfHeight*2));//这个函数不会重置原始矩形的大小
			//            }
			//            else
			//            {
			//                setRectSize(m_RECT);
			//            }
		}
		else if (m_StateFlag == CALI_RECT_BR)//右下角顶点
		{
			QPointF pc, p0, p1, p2, p3;//,pCross1,pCross2;
			pc = m_RotateCenter;
			p0 = m_RECT_Pol[0];
			p1 = m_RECT_Pol[1];
			p2 = m_RECT_Pol[2];
			p3 = m_RECT_Pol[3];
			if (!get2CrossPt(p0, p1, p2, pc, event->pos(), pCross1, pCross2))
				return;
			//            //该段代码可禁止矩形被反向拉伸！！！
			//            double disVertical2RightLine = CalDisPoint2longLine(event->pos().x(),event->pos().y(),p1.x(),p1.y(),p2.x(),p2.y());
			//            double disVertical2LeftLine = CalDisPoint2longLine(event->pos().x(),event->pos().y(),p0.x(),p0.y(),p3.x(),p3.y());
			//            if(disVertical2RightLine>disVertical2LeftLine)
			//                return;
			//            double disVertical2BottomLine = CalDisPoint2longLine(event->pos().x(),event->pos().y(),p2.x(),p2.y(),p3.x(),p3.y());
			//            double disVertical2TopLine = CalDisPoint2longLine(event->pos().x(),event->pos().y(),p0.x(),p0.y(),p1.x(),p1.y());
			//            if(disVertical2BottomLine>disVertical2TopLine)
			//                return;
			//            //该段代码可禁止矩形被反向拉伸！！！
			if (m_bKeepShadowLength && m_fShearX != 0)//保证投影长度不变
			{
				double disp2pc = CalDis(m_RECTShear[2].x(), m_RECTShear[2].y(), pc.x(), pc.y());
				double dispospc = CalDis(event->pos().x(), event->pos().y(), pc.x(), pc.y());
				double newHeight = dispospc / disp2pc * m_newShearRECT.height();
				double newWidth = dispospc / disp2pc * m_newShearRECT.width();
				setShearRectSize(QRectF(pc.x() - newWidth / 2, pc.y() - newHeight / 2, newWidth, newHeight));
				double newHeightOld = dispospc / disp2pc * m_RECT.height();
				double newWidthOld = dispospc / disp2pc * m_RECT.width();
				m_RECT = QRectF(pc.x() - newWidthOld / 2, pc.y() - newHeightOld / 2, newWidthOld, newHeightOld);
			}
			else//投影长度可变
			{
				//新矩形宽度一半：
				double dispcpCross1 = CalDis(pc.x(), pc.y(), pCross1.x(), pCross1.y());
				if (dispcpCross1 < 10)
					return;//矩形宽度不能小于20
				//新矩形高度一半：
				double disVertical = CalDisPoint2longLine(event->pos().x(), event->pos().y(), pc.x(), pc.y(), pCross1.x(), pCross1.y());
				if (disVertical < 10)
					return;//矩形高度不能小于20
				setRectSize(QRectF(pc.x() - dispcpCross1, pc.y() - disVertical, dispcpCross1 * 2, disVertical * 2));

			}
			scene()->update();
		}
	}
}

void GraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::ArrowCursor);
	m_StateFlag = CALI_DEFAULT;
	//    if(m_StateFlag == CALI_RECT_MOVE)
	//    {
	//        m_StateFlag = CALI_DEFAULT;
	//    }
	//    else {
	//        QGraphicsItem::mouseReleaseEvent(event);
	//    }
	QGraphicsItem::mouseReleaseEvent(event);
	//setSelected(false);
}

int GraphicsRectItem::type() const
{
	return UserType + 1;//表示GraphicsRectItem
}

void GraphicsRectItem::SetRotate(qreal RotateAngle)
{
	m_RotateAngle = RotateAngle;
	if (m_bKeepShadowLength && m_newShearRECT.width() != 0)
	{
		setShearRectSize(m_newShearRECT);
	}
	else
	{
		setRectSize(m_RECT);
	}
	if (this->scene() != nullptr)
		this->scene()->update();
}

void GraphicsRectItem::SetSideCursorByRotateAngle(qreal RotateAngle)
{
	if ((RotateAngle > 45 && RotateAngle < 135) || (RotateAngle > 225 && RotateAngle < 315))
	{
		if (m_StateFlag == CALI_RECT_TR || m_StateFlag == CALI_RECT_BL)
		{
			setCursor(Qt::SizeFDiagCursor);
		}
		else if (m_StateFlag == CALI_RECT_TL || m_StateFlag == CALI_RECT_BR)
		{
			setCursor(Qt::SizeBDiagCursor);
		}
	}
	else
	{
		if (m_StateFlag == CALI_RECT_TR || m_StateFlag == CALI_RECT_BL)
		{
			setCursor(Qt::SizeBDiagCursor);
		}
		else if (m_StateFlag == CALI_RECT_TL || m_StateFlag == CALI_RECT_BR)
		{
			setCursor(Qt::SizeFDiagCursor);
		}
	}
}

QPointF GraphicsRectItem::getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle)
{
	double dx = ptCenter.x();
	double dy = ptCenter.y();
	double x = ptIn.x();
	double y = ptIn.y();
	double xx, yy;
	xx = (x - dx)*cos(angle*M_PI / 180) - (y - dy)*sin(angle*M_PI / 180) + dx;
	yy = (x - dx)*sin(angle*M_PI / 180) + (y - dy)*cos(angle*M_PI / 180) + dy;

	return QPointF(xx, yy);
}

QList<QPointF> GraphicsRectItem::getRotatePoints(QPointF ptCenter, QList<QPointF> ptIns, qreal angle)
{
	QList<QPointF> lstPt;
	for (int i = 0; i < ptIns.count(); i++)
	{
		lstPt.append(getRotatePoint(ptCenter, ptIns.at(i), angle));
	}
	return lstPt;
}

QPolygonF GraphicsRectItem::getRotatePolygonFromRect(QPointF ptCenter, QRectF rectIn, qreal angle)
{
	QVector<QPointF> vpt;
	QPointF pf = getRotatePoint(ptCenter, rectIn.topLeft(), angle);
	vpt.append(pf);
	pf = getRotatePoint(ptCenter, rectIn.topRight(), angle);
	vpt.append(pf);
	pf = getRotatePoint(ptCenter, rectIn.bottomRight(), angle);
	vpt.append(pf);
	pf = getRotatePoint(ptCenter, rectIn.bottomLeft(), angle);
	vpt.append(pf);
	pf = getRotatePoint(ptCenter, rectIn.topLeft(), angle);
	vpt.append(pf);
	return QPolygonF(vpt);
}

QPolygonF GraphicsRectItem::getRotatePolygonFromPolygon(QPointF ptCenter, QPolygonF polyIn, qreal angle)
{
	QVector<QPointF> vpt;
	for (int i = 0; i < polyIn.count(); i++)
	{
		QPointF pf = getRotatePoint(ptCenter, polyIn[i], angle);
		vpt.append(pf);
	}
	return QPolygonF(vpt);
}

QRectF GraphicsRectItem::getBoundedRectToSceen()
{
	return QRectF(m_RECT_Pol.boundingRect().x() + pos().x(), m_RECT_Pol.boundingRect().y() + pos().y(), m_RECT_Pol.boundingRect().width(), m_RECT_Pol.boundingRect().height());
}

QPolygonF GraphicsRectItem::getCrtPolygonToScreen()
{
	QVector<QPointF> vpt;
	if (m_ShapeType == CALIPER_ELLIPSE)
	{
		for (int i = 0; i < m_RECT_Pol.length(); i++)
		{
			vpt.append(QPointF(m_RECT_Pol[i].x() + pos().x(), m_RECT_Pol[i].y() + pos().y()));
		}
	}
	return QPolygonF(vpt);
}
QRectF GraphicsRectItem::getSmallRotateRect(QPointF ptA, QPointF ptB)
{
	QPointF pt = getSmallRotateRectCenter(ptA, ptB);
	return QRectF(pt.x() - 10, pt.y() - 10, 20, 20);
}

QPointF GraphicsRectItem::getSmallRotateRectCenter(QPointF ptA, QPointF ptB)
{
	QPointF ptCenter = QPointF((ptA.x() + ptB.x()) / 2, (ptA.y() + ptB.y()) / 2);//A,B点的中点C
	//中垂线方程式为 y=x*k + b;
	qreal x, y;//旋转图标矩形的中心
	if (abs(ptB.y() - ptA.y()) < 0.1)
	{
		if (ptA.x() < ptB.x())//矩形左上角在上方
		{
			x = ptCenter.x();
			y = ptCenter.y() - 20;
		}
		else//矩形左上角在下方
		{
			x = ptCenter.x();
			y = ptCenter.y() + 20;
		}
	}
	else if (ptB.y() > ptA.y())//顺时针旋转0-180
	{
		qreal k = (ptA.x() - ptB.x()) / (ptB.y() - ptA.y());//中垂线斜率
		qreal b = (ptA.y() + ptB.y()) / 2 - k * (ptA.x() + ptB.x()) / 2;
		//求AB线中垂线上离AB中点20个像素的点C的坐标
		x = 20 * cos(atan(k)) + ptCenter.x();
		y = k * x + b;
	}
	else if (ptB.y() < ptA.y())//顺时针旋转180-360
	{
		qreal k = (ptA.x() - ptB.x()) / (ptB.y() - ptA.y());//中垂线斜率
		qreal b = (ptA.y() + ptB.y()) / 2 - k * (ptA.x() + ptB.x()) / 2;
		//求AB线中垂线上离AB中点20个像素的点C的坐标
		x = -20 * cos(atan(k)) + ptCenter.x();
		y = k * x + b;
	}
	return QPointF(x, y);
}
