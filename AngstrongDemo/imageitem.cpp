#include "ImageItem.h"

ImageItem::ImageItem(QGraphicsItem *parent /* = nullptr */)
	:QGraphicsItem(parent)
{
	setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

ImageItem::~ImageItem()
{
}

QRectF ImageItem::boundingRect() const
{
	return QRectF();
}

void ImageItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
}

void ImageItem::wheelEvent(QGraphicsSceneWheelEvent * event)
{
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
	
}

void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	
}
