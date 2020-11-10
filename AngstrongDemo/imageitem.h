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
public:
	void SetImage(QImage qImage);
	void SetImage(QPixmap *pix);
	void Zoom(QPointF pointF, double fScale = 1.0f);
	void ZoomIn(QPointF poinF, double fScale = 1.2f);
	void ZoomOut(QPointF pointF, double fScale = 0.8f);
	void ZoomFit();
	void ResetItemPos();
	void setQGraphicsViewWH(int nwidth, int nheight);

	int GetImageWidth()const;
	int GetImageHeight()const;
protected:
	QRectF  boundingRect() const;
	void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent(QGraphicsSceneWheelEvent *event);

	bool sceneEvent(QEvent *event);


private:
	float m_fScale;
	bool m_bIsMove;
	QPointF m_startPos;
	QPixmap m_pix;

	int m_ImageWidth;
	int m_ImageHeight;
};

