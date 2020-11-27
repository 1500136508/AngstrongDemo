#include "imagescene.h"

ImageScene::ImageScene(QObject *parent /* = nullptr */)
	:QGraphicsScene(parent)
{
	setBackgroundBrush(QBrush(QColor(128, 128, 128)));
}
ImageScene::ImageScene(const QRectF &sceneRect, QObject *parent /* = nullptr */)
	: QGraphicsScene(sceneRect, parent)
{

}

ImageScene::ImageScene(qreal x, qreal y, qreal width, qreal height, QObject *parent /* = nullptr */)
	: QGraphicsScene(x, y, width, height, parent)
{

}

ImageScene::~ImageScene()
{
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	update();
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	update();
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	update();
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void ImageScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{
	QGraphicsScene::wheelEvent(wheelEvent);
}
