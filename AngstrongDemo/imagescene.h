#pragma once
#include <qgraphicsscene.h>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
class ImageScene : public QGraphicsScene
{
	Q_OBJECT
public:
	ImageScene(QObject *parent = nullptr);
	ImageScene(const QRectF &sceneRect, QObject *parent = nullptr);
	ImageScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr);
	virtual ~ImageScene();

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
};

