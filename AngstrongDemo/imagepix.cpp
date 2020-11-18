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
	//int x = QCursor::pos().x();
	//int y = QCursor::pos().y();

	//QPixmap pixmap = QPixmap::fromImage(qImage);
	//if (!pixmap.isNull()) //�������ͼ��ΪNULL
	//{
	//	QImage image = pixmap.toImage();//������ͼת��ΪQImage
	//	if (!image.isNull()) //���image��Ϊ��
	//	{
	//		if (image.valid(0, 0)) //����λ����Ч
	//		{
	//			QColor color = image.pixel(0, 0);
	//			int mousedPressed_R = color.red();
	//			int mousedPressed_G = color.green();
	//			int mousedPressed_B = color.blue();
	//			//int text = QString("RGB: %1, %2, %3").arg(mousedPressed_R).arg(mousedPressed_G).arg(mousedPressed_B);
	//			//qDebug() << text;
	//		}
	//	}
	//}

	//int a = 6;
	return QGraphicsPixmapItem::mouseMoveEvent(event);
}

void ImagePix::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	if ((event->delta() > 0) && (m_fScale >= 50))//���Ŵ�ԭʼͼ���50��
	{
		return QGraphicsPixmapItem::wheelEvent(event);
	}
	else if ((event->delta() < 0) && (m_fScale <= 0.5))//ͼ����С������Ӧ��С֮��Ͳ�������С
	{
		//ResetItemPos();//����ͼƬ��С��λ�ã�ʹ֮����Ӧ�ؼ����ڴ�С
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
