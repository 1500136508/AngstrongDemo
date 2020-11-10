#include "ImageItem.h"

ImageItem::ImageItem(QGraphicsItem *parent /* = nullptr */)
	:QGraphicsItem(parent)
{
	m_fScale = 1.0f;
	m_bIsMove = false;

	m_ImageWidth = 0;
	m_ImageHeight = 0;
	//setAcceptHoverEvents(true);
	//setAcceptedMouseButtons(Qt::LeftButton);
	//setFlag(QGraphicsItem::ItemIsSelectable);//必须加上这句，否则item无法获取到鼠标事件
	//setFlags(ItemIsSelectable | ItemIsMovable);
	//setFlag(QGraphicsItem::ItemIsFocusable);
	//setFlag(QGraphicsItem::ItemIsMovable);

	setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

ImageItem::~ImageItem()
{
}

void ImageItem::SetImage(QImage qImage)
{
	m_pix = QPixmap::fromImage(qImage);

	m_ImageWidth = m_pix.width();
	m_ImageHeight = m_pix.height();
}

void ImageItem::SetImage(QPixmap * pix)
{
	m_pix = *pix;
}

void ImageItem::Zoom(QPointF pointF, double fScale)
{
}

void ImageItem::ZoomIn(QPointF poinF, double fScale)
{
}

void ImageItem::ZoomOut(QPointF pointF, double fScale)
{
}

void ImageItem::ZoomFit()
{
}

void ImageItem::ResetItemPos()
{
	m_fScale = 1.0f;//缩放比例回到一开始的自适应比例
	setScale(m_fScale);//缩放到一开始的自适应大小
	setPos(0, 0);
}

void ImageItem::setQGraphicsViewWH(int nwidth, int nheight)
{
	int nImgWidth = m_ImageWidth;
	int nImgHeight = m_ImageHeight;
	qreal temp1 = nwidth * 1.0 / nImgWidth;
	qreal temp2 = nheight * 1.0 / nImgHeight;
	if (temp1 > temp2)
	{
		m_fScale = temp2;
	}
	else
	{
		m_fScale = temp1;
	}
	setScale(m_fScale);
	m_fScale = m_fScale;
}

int ImageItem::GetImageWidth() const
{
	return m_ImageWidth;
}

int ImageItem::GetImageHeight() const
{
	return m_ImageHeight;
}

QRectF ImageItem::boundingRect() const
{
	//return QRectF(-m_ImageWidth / 2, -m_ImageHeight / 2,
	//	m_ImageWidth, m_ImageHeight);
	return QRectF(-m_pix.width() / 2, -m_pix.height() / 2,
				m_pix.width(), m_pix.height());
	//QRectF rect;
	//if (m_pix.size() != QSize(0,0))
	//{
	//	rect = QRectF(-m_pix.width() / 2, -m_pix.height() / 2,
	//		m_pix.width(), m_pix.height());
	//}
	//else
	//{
	//	rect = QRectF(-100, -100, 1000, 1000);
	//}

	//return rect;
}

void ImageItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	//painter->drawPixmap(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);
	painter->drawPixmap(-m_ImageWidth / 2, -m_ImageHeight / 2, m_pix);
	//painter->drawPixmap(m_ImageWidth, m_ImageHeight, m_pix);
	/*if (hasFocus()) {
		painter->setPen(QPen(QColor(255, 255, 255, 200)));
	}
	else {
		painter->setPen(QPen(QColor(100, 100, 100, 100)));
	}
	painter->setBrush(Qt::red);
	painter->drawRect(-10, -10, 20, 20);*/
}

void ImageItem::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	if ((event->delta() > 0) && (m_fScale >= 50))//最大放大到原始图像的50倍
	{
		return;
	}
	else if ((event->delta() < 0) && (m_fScale <= 0.5))//图像缩小到自适应大小之后就不继续缩小
	{
		//ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
	}
	else
	{
		qreal qrealOriginScale = m_fScale;
		if (event->delta() > 0)//鼠标滚轮向前滚动
		{
			m_fScale *= 1.1;//每次放大10%
		}
		else
		{
			m_fScale *= 0.9;//每次缩小10%
		}
		setScale(m_fScale);
		if (event->delta() > 0)
		{
			moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
		}
		else
		{
			moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
		}
	}
}

bool ImageItem::sceneEvent(QEvent * event)
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
	else if (event->type() == QEvent::GraphicsSceneWheel)
	{
		wheelEvent((QGraphicsSceneWheelEvent *)(event));
	}

	event->setAccepted(true);
	return true;
}

void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		event->accept();
	}

	if (event->button() == Qt::LeftButton)
	{
		m_startPos = event->pos();//鼠标左击时，获取当前鼠标在图片中的坐标，
		m_bIsMove = true;//标记鼠标左键被按下
	}
	else if (event->button() == Qt::RightButton)
	{
		ResetItemPos();//右击鼠标重置大小
	}
}

void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	if (m_bIsMove)
	{
		QPointF point = (event->pos() - m_startPos)*m_fScale;
		moveBy(point.x(), point.y());
	}
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	m_bIsMove = false;//标记鼠标左键已经抬起
}
