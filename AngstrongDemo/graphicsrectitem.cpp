#include "graphicsrectitem.h"
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QStandardPaths>
#include <QGraphicsSceneHoverEvent>

#pragma execution_character_set("utf-8")//让能够正常显示中文字符串

GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent)
{
	setCursor(Qt::ArrowCursor);   //改变光标形状,手的形状

	initViewer();
	setAcceptHoverEvents(true);
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
}

GraphicsRectItem::~GraphicsRectItem()
{
}

QRect GraphicsRectItem::getRoiRect() const
{
	return m_roiRect;
}

void GraphicsRectItem::setBackImage(const QImage & img)
{
	m_backImage = img;
	//setMinimumSize(img.width(), img.height());
	update();
}

void GraphicsRectItem::setROIRect(QRect rect)
{
	m_roiRect = rect;
}

QRectF GraphicsRectItem::boundingRect() const
{
	return QRectF(m_roiRect.x()-1,m_roiRect.y()-1,m_roiRect.width()+1,m_roiRect.height()+1);
}

void GraphicsRectItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	/*if (m_backImage.isNull())
		return;*/

	QPixmap rawImg = QPixmap::fromImage(m_backImage);
	QPointF qPointF = mapToScene(m_roiRect.topLeft().x(), m_roiRect.topLeft().y());
	QString strPoint = QString("X:%0, Y:%1").arg(qPointF.x()).arg(qPointF.y());           //位置信息
	QString strSize = QString("W:%0, H:%1").arg(m_roiRect.width()).arg(m_roiRect.height());   //大小信息

	QPen pen;
	pen.setColor(Qt::yellow);
	pen.setWidth(EDGE_WIDTH);

	QFont font;
	font.setPixelSize(16);

	//QPainter painter;
	//painter.begin(this);
	//painter->setBrush(QBrush(QColor(0, 0, 200, 120)));
	painter->setPen(pen);
	painter->setFont(font);
	painter->drawText(m_roiRect.bottomLeft().x()+m_roiRect.width()/4, m_roiRect.bottomLeft().y() + 35, strSize);
	painter->drawText(m_roiRect.bottomLeft().x() + m_roiRect.width() / 4, m_roiRect.bottomLeft().y() + 20, strPoint);
	painter->drawRect(m_roiRect);

	if (m_roiRect.width() != 0 && m_roiRect.height() != 0)
	{
#ifdef DRAW_SUB_LINE
		//绘制中间十字架
		/*QPen dashPen(Qt::white);
		dashPen.setWidth(MIDDLELINE_WIDTH);
		dashPen.setStyle(Qt::DashDotLine);
		painter->setPen(dashPen);
		painter->drawLine(m_roiRect.topLeft().x() + m_roiRect.width() / 2, m_roiRect.topLeft().y() + EDGE_WIDTH, m_roiRect.bottomRight().x() - m_roiRect.width() / 2, m_roiRect.bottomRight().y());
		painter->drawLine(m_roiRect.topLeft().x() + EDGE_WIDTH, m_roiRect.topLeft().y() + m_roiRect.height() / 2, m_roiRect.bottomRight().x(), m_roiRect.bottomRight().y() - m_roiRect.height() / 2);*/
#endif

#ifdef DRAW_TEN_POINT
		//绘制边缘十个点
		painter->setBrush(Qt::green);
		pen.setWidth(0);
		painter->setPen(pen);

		painter->drawRect(m_roiRect.topLeft().x()- POINT_WIDTH/2, m_roiRect.topLeft().y()- POINT_HEIGHT/2, POINT_WIDTH, POINT_HEIGHT); //左上角
		painter->drawRect(m_roiRect.topLeft().x() - POINT_WIDTH / 2, m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_WIDTH / 2, POINT_WIDTH, POINT_HEIGHT); //左边中心点
		painter->drawRect(m_roiRect.bottomLeft().x() - POINT_WIDTH / 2, m_roiRect.bottomLeft().y() - POINT_WIDTH+ POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //左下角
		painter->drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y()- POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT);  //顶部中心
		painter->drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_WIDTH / 2, POINT_WIDTH, POINT_HEIGHT);  //中心点
		painter->drawRect(m_roiRect.bottomLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.bottomLeft().y() - POINT_WIDTH+ POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //底部中心点
		painter->drawRect(m_roiRect.topRight().x() - POINT_WIDTH+ POINT_WIDTH / 2, m_roiRect.topRight().y()- POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //右上角
		painter->drawRect(m_roiRect.topRight().x() - POINT_WIDTH / 2/*+ POINT_WIDTH / 2*/, m_roiRect.topRight().y() + m_roiRect.height() / 2 - POINT_WIDTH / 2, POINT_WIDTH, POINT_HEIGHT); //右边中心点
		painter->drawRect(m_roiRect.bottomRight().x() - POINT_WIDTH+ POINT_WIDTH / 2, m_roiRect.bottomRight().y() - POINT_WIDTH+ POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //右下角点
#endif
	}


	//painter.end();

	qDebug() << m_roiRect;
	//#else
	//	/*QLabel::paintEvent(event);
	//
	//	if (m_backImage.isNull())
	//	return;
	//	QPixmap rawImg = QPixmap::fromImage(m_backImage);
	//	QPainter painter(this);
	//	painter.begin(&rawImg);
	//	painter.setBrush(Qt::gray);
	//	painter.drawRect(30, 30, 100, 100);
	//	painter.end();
	//	this->setPixmap(rawImg);*/
	//#endif
}

void GraphicsRectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
	QPoint mousePos = event->pos().toPoint();

	if (m_roiRect.contains(mousePos))
	{
		m_pOptMenu->exec(QCursor::pos());
	}

	event->accept();
}

void GraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		if (m_bPainterPressed)
		{
			//正在绘制状态
			paintRect(event->pos().toPoint());

		}
		else if (m_bMovedPressed)
		{
			//正在移动状态
			moveRect(event->pos().toPoint());
		}
		else if (m_bScalePressed)
		{
			//正在缩放大小状态
			scaleRect(event->pos().toPoint());
		}

		//更新界面
		update();
		return;
	}

	//根据鼠标的位置设置当前的鼠标形状
	EmDirection dir = region(event->pos().toPoint());

	if (dir == DIR_NONE)
	{
		setCursor(Qt::ArrowCursor);
	}
	else if (dir == DIR_MIDDLE)
	{
		setCursor(Qt::OpenHandCursor);
	}
}

void GraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		EmDirection dir = region(event->pos().toPoint());     //获取鼠标当前的位置

		if (dir == DIR_MIDDLE)
		{
			//鼠标在矩形中心位置
			this->setCursor(Qt::ClosedHandCursor);
			m_moveStartPoint.setX(event->pos().x());
			m_moveStartPoint.setY(event->pos().y());
			m_bMovedPressed = true;
		}
		else if (dir == DIR_NONE)
		{
			//鼠标在矩形外部
			this->setCursor(Qt::ArrowCursor);
			m_bPainterPressed = true;
			m_paintStartPoint.setX(event->pos().x());
			m_paintStartPoint.setY(event->pos().y());
		}
		else
		{
			//矩形在矩形边缘
			m_moveStartPoint.setX(event->pos().x());
			m_moveStartPoint.setY(event->pos().y());
			m_bScalePressed = true;
			m_emCurDir = dir;
		}
	}
}

void GraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	//判断鼠标是否在矩形中
	if (m_roiRect.contains(event->pos().toPoint()))
	{
		//松开鼠标前是否正在拖放
		if (m_bMovedPressed)
		{
			this->setCursor(Qt::OpenHandCursor);
		}
		else
		{
			this->setCursor(Qt::ArrowCursor);
		}
	}

	m_paintStartPoint = QPoint();
	m_bMovedPressed = false;
	m_bPainterPressed = false;
	m_bScalePressed = false;
}

void GraphicsRectItem::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Delete)
	{
		m_roiRect = QRect(0, 0, 0, 0);
		update();
	}
}

void GraphicsRectItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	//根据鼠标的位置设置当前的鼠标形状
	EmDirection dir = region(event->pos().toPoint());

	if (dir == DIR_NONE)
	{
		setCursor(Qt::ArrowCursor);
	}
	else if (dir == DIR_MIDDLE)
	{
		setCursor(Qt::OpenHandCursor);
	}
	update();
}

void GraphicsRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	//根据鼠标的位置设置当前的鼠标形状
	EmDirection dir = region(event->pos().toPoint());

	if (dir == DIR_NONE)
	{
		setCursor(Qt::ArrowCursor);
	}
	else if (dir == DIR_MIDDLE)
	{
		setCursor(Qt::OpenHandCursor);
	}
	update();
}

void GraphicsRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
	//根据鼠标的位置设置当前的鼠标形状
	EmDirection dir = region(event->pos().toPoint());

	if (dir == DIR_NONE)
	{
		setCursor(Qt::ArrowCursor);
	}
	else if (dir == DIR_MIDDLE)
	{
		setCursor(Qt::OpenHandCursor);
	}
	update();
}

bool GraphicsRectItem::sceneEvent(QEvent * event)
{
	//return false;
	if (event->type() == QEvent::GraphicsSceneMousePress)
	{
		mousePressEvent((QGraphicsSceneMouseEvent *)(event));
	}
	else if (event->type() == QEvent::GraphicsSceneMouseRelease)
	{
		mouseReleaseEvent((QGraphicsSceneMouseEvent *)(event));
	}
	else if (event->type() == QEvent::GraphicsSceneMouseMove)
	{
		mouseMoveEvent((QGraphicsSceneMouseEvent *)(event));
	}
	else if (event->type() == QEvent::KeyPress)
	{
		keyPressEvent((QKeyEvent *)(event));
	}
	else if (event->type() == QEvent::GraphicsSceneWheel)
	{
		wheelEvent((QGraphicsSceneWheelEvent *)(event));
	}

	event->setAccepted(true);
	return QGraphicsItem::sceneEvent(event);
}

void GraphicsRectItem::initViewer()
{
	m_bPainterPressed = false;
	m_bMovedPressed = false;
	m_bScalePressed = false;
	m_roiRect = QRect(50, 50, 220, 220);
	m_emCurDir = EmDirection::DIR_NONE;

	/*this->setMouseTracking(true);
	this->setFocusPolicy(Qt::StrongFocus);*/

	m_pOptMenu = new QMenu();
	m_pDelAction = new QAction(QStringLiteral("删除"), this);
	connect(m_pDelAction, SIGNAL(triggered()), this, SLOT([&]() { m_roiRect = QRect(0, 0, 0, 0); }));
	m_pSaveAction = new QAction(QStringLiteral("保存"), this);
	connect(m_pSaveAction, SIGNAL(triggered()), this, SLOT(saveROIImage()));

	m_pOptMenu->addAction(m_pDelAction);
	m_pOptMenu->addAction(m_pSaveAction);
}

void GraphicsRectItem::saveROIImage()
{
	QString fileName = QFileDialog::getSaveFileName(nullptr, "Save", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "*.png");
	if (fileName.isEmpty())
	{
		return;
	}

	QImage saveImg = m_backImage.copy(m_roiRect);
	saveImg.save(fileName);
}

EmDirection GraphicsRectItem::region(QPoint point)
{
	int mouseX = point.x();
	int mouseY = point.y();

	QPoint roiTopLeft = m_roiRect.topLeft();
	QPoint roiBottomRight = m_roiRect.bottomRight();

	EmDirection dir = DIR_NONE;

	if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
	{
		//左上角
		this->setCursor(Qt::SizeFDiagCursor);
		dir = DIR_LEFTTOP;
	}
	else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
	{
		//右上角
		this->setCursor(Qt::SizeBDiagCursor);
		dir = DIR_RIGHTTOP;
	}
	else if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
	{
		//左下角
		this->setCursor(Qt::SizeBDiagCursor);
		dir = DIR_LEFTBOTTOM;
	}
	else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
	{
		//右下角
		this->setCursor(Qt::SizeFDiagCursor);
		dir = DIR_RIGHTBOTTOM;
	}
	else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
	{
		//右边
		this->setCursor(Qt::SizeHorCursor);
		dir = DIR_RIGHT;
	}
	else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
	{
		//上边
		this->setCursor(Qt::SizeVerCursor);
		dir = DIR_TOP;
	}
	else if (mouseY >= roiBottomRight.y() - EDGPADDING && mouseY <= roiBottomRight.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
	{
		//下边
		this->setCursor(Qt::SizeVerCursor);
		dir = DIR_BOTTOM;
	}
	else if (mouseX <= roiTopLeft.x() + EDGPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
	{
		//左边
		this->setCursor(Qt::SizeHorCursor);
		dir = DIR_LEFT;
	}
	else if (m_roiRect.contains(point))
	{
		//中间
		dir = DIR_MIDDLE;
	}
	else
	{
		dir = DIR_NONE;
	}

	return dir;
}

void GraphicsRectItem::scaleRect(const QPoint & mousePoint)
{
	QRect newRect(m_roiRect.topLeft(), m_roiRect.bottomRight());
	int width = mousePoint.x() - m_moveStartPoint.x();   //移动的宽度
	int height = mousePoint.y() - m_moveStartPoint.y();  //移动的高度

	//根据当前的缩放状态来改变矩形的位置大小信息
	switch (m_emCurDir)
	{
	case DIR_LEFT:
		newRect.setLeft(mousePoint.x());
		break;
	case DIR_RIGHT:
		newRect.setRight(mousePoint.x());
		break;
	case DIR_TOP:
		newRect.setTop(mousePoint.y());
		break;
	case DIR_BOTTOM:
		newRect.setBottom(mousePoint.y());
		break;
	case DIR_LEFTTOP:
		newRect.setTopLeft(mousePoint);
		break;
	case DIR_LEFTBOTTOM:
		newRect.setBottomLeft(mousePoint);
		break;
	case DIR_RIGHTTOP:
		newRect.setTopRight(mousePoint);
		break;
	case DIR_RIGHTBOTTOM:
		newRect.setBottomRight(mousePoint);
		break;
	}

	if (newRect.width() < MIN_WIDTH || newRect.height() < MIN_HEIGHT)
	{
		//缩放的大小限制
		return;
	}

	m_roiRect = newRect;
	m_moveStartPoint = mousePoint;  //更新鼠标的起始位置
}

void GraphicsRectItem::paintRect(const QPoint & mousePoint)
{
	this->setCursor(Qt::ArrowCursor);                    //设置鼠标为指针形状

	int width = mousePoint.x() - m_paintStartPoint.x();  //移动的宽度
	int height = mousePoint.y() - m_paintStartPoint.y(); //移动的高度

	if (width < 0 && height < 0)
	{
		//鼠标向左上角移动
		m_roiRect.setX(mousePoint.x());
		m_roiRect.setY(mousePoint.y());
	}
	else if (width < 0)
	{
		//鼠标向负X位置移动
		m_roiRect.setX(mousePoint.x());
		m_roiRect.setY(m_paintStartPoint.y());
	}
	else if (height < 0)
	{
		//鼠标向负Y位置移动
		m_roiRect.setX(m_paintStartPoint.x());
		m_roiRect.setY(mousePoint.y());
	}
	else
	{
		//正常  向右下移动
		m_roiRect.setX(m_paintStartPoint.x());
		m_roiRect.setY(m_paintStartPoint.y());
	}

	//设置矩形大小 绝对值 避免反方向的产生的负值
	m_roiRect.setSize(QSize(abs(width), abs(height)));
}

void GraphicsRectItem::moveRect(const QPoint & mousePoint)
{
	this->setCursor(Qt::ClosedHandCursor);

	int width = mousePoint.x() - m_moveStartPoint.x();
	int height = mousePoint.y() - m_moveStartPoint.y();

	QRect ret;
	ret.setX(m_roiRect.x() + width);
	ret.setY(m_roiRect.y() + height);
	ret.setSize(m_roiRect.size());
	m_roiRect = ret;
	m_moveStartPoint = mousePoint;
}
