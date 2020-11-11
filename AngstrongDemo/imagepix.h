#pragma once
#include <qgraphicsitem.h>

class ImagePix :public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	explicit ImagePix(QGraphicsPixmapItem *parent = nullptr);
	explicit ImagePix(const QPixmap &pixmap, QGraphicsPixmapItem *parent = nullptr);
	virtual ~ImagePix();
};

