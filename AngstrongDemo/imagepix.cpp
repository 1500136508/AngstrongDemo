#include <QGraphicsSceneWheelEvent>
#include <QDebug>
#include "imagepix.h"

ImagePix::ImagePix(QGraphicsPixmapItem *parent /* = nullptr */)
	:QGraphicsPixmapItem(parent)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsFocusable);
	//setFlag(QGraphicsItem::ItemIsSelectable);

	m_fScale = 1.0f;
}

ImagePix::ImagePix(const QPixmap &pixmap, QGraphicsPixmapItem *parent /* = nullptr */)
	: QGraphicsPixmapItem(pixmap, parent)
{
	
}

ImagePix::~ImagePix()
{
}

void ImagePix::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	return QGraphicsPixmapItem::mouseMoveEvent(event);
}

void ImagePix::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	if ((event->delta() > 0) && (m_fScale >= 80))//���Ŵ�ԭʼͼ���80��
	{
		return QGraphicsPixmapItem::wheelEvent(event);
	}
	else if ((event->delta() < 0) && (m_fScale <= 0.5))//ͼ����С������Ӧ��С֮��Ͳ�������С
	{
	}
	else
	{
		qreal qrealOriginScale = m_fScale;
		if (event->delta() > 0)//��������ǰ����
		{
			m_fScale *= 1.1;//ÿ�ηŴ�10%
		}
		else
		{
			m_fScale *= 0.9;//ÿ����С10%
		}
		setScale(m_fScale);
		if (event->delta() > 0)
		{
			moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
		}
		else
		{
			moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
		}
	}

	return QGraphicsPixmapItem::wheelEvent(event);
}

void ImagePix::SetFit(double fScale)
{
	m_fScale = fScale;
	setScale(m_fScale);
}

float ImagePix::GetScale() const
{
	return m_fScale;
}
