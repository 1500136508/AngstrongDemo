#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <core.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>

class View : public QGraphicsView
{
	Q_OBJECT
public:
	View(QWidget *pParent = nullptr);
	View(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~View();
protected:
	void contextMenuEvent(QContextMenuEvent *ev);
private slots:
	void on_Open_triggle();
	void on_Save_triggle();
	void on_Close_triggle();
public:
	bool Open();
	bool Save();
	bool Close();

private:
	cv::Mat m_Image;
	QGraphicsScene * m_scene;
	//»­²¼
	QGraphicsPixmapItem * m_imageItem;
};

