#pragma once
#include <qgraphicsitem.h>
#include <QPointF>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>

class ImageItem : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	explicit ImageItem(QGraphicsItem *parent = nullptr);
	virtual ~ImageItem();
protected:
	QRectF  boundingRect() const;
	void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent(QGraphicsSceneWheelEvent *event);

	bool sceneEvent(QEvent *event);
};

