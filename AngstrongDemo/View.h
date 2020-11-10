#pragma once

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <core.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>
#include "imagereader.h"
#include "imageitem.h"
#include "imagescene.h"

class View : public QGraphicsView
{
	Q_OBJECT
public:
	View(QWidget *pParent = nullptr);
	View(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~View();
protected:
	//void contextMenuEvent(QContextMenuEvent *ev);

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);


	void paintEvent(QPaintEvent *event);

public slots:
	void on_Open_triggle();
	void on_Save_triggle();
	void on_Close_triggle();

	void setImage(cv::Mat mat);
public:
	bool Open();
	bool Save();
	bool Close();

	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);
public:
	cv::Mat m_Image;
	ImageScene *m_scene;//����
	ImageItem *m_ImageItem;
	imageReader *m_reader;
};

