#include <QGraphicsSceneWheelEvent>
#include "imagepix.h"

ImagePix::ImagePix(QGraphicsPixmapItem *parent /* = nullptr */)
	:QGraphicsPixmapItem(parent)
{
	setFlag(QGraphicsItem::ItemIsMovable);

	m_fScale = 1.0f;
}

ImagePix::ImagePix(const QPixmap &pixmap, QGraphicsPixmapItem *parent /* = nullptr */)
	: QGraphicsPixmapItem(pixmap, parent)
{
	
}

ImagePix::~ImagePix()
{
}

void ImagePix::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	if ((event->delta() > 0) && (m_fScale >= 50))//最大放大到原始图像的50倍
	{
		return QGraphicsPixmapItem::wheelEvent(event);
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

	return QGraphicsPixmapItem::wheelEvent(event);
}
