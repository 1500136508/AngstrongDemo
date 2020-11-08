#include "imagescene.h"

ImageScene::ImageScene(QObject *parent /* = nullptr */)
	:QGraphicsScene(parent)
{

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
	int a;
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	int b;
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	int c;
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void ImageScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{
	int d;
	QGraphicsScene::wheelEvent(wheelEvent);
}
