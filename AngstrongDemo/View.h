#pragma once

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <core.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>
#include "imagereader.h"
#include "imagitem.h"
#include "imagescene.h"

class View : public QGraphicsView
{
	Q_OBJECT
public:
	View(QWidget *pParent = nullptr);
	View(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~View();
protected:
	void contextMenuEvent(QContextMenuEvent *ev);

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);


	//void paintEvent(QPaintEvent *event);

private slots:
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
	ImageScene *m_scene;//³¡¾°
	ImagItem *m_ImageItem;
	imageReader *m_reader;


	//only copy test;
private:
	cv::Point startPoint;
	cv::Point endPoint;
	cv::Point startPoint2;
	cv::Point endPoint2;
	bool getFirstArea = false;
	bool getSecondArea = false;

	cv::Point pos;
	QPoint m_ptPress;
	QRect m_areaMovable;

	bool showValue = false;
	int boundaryLength = 0;
	float ratio = 0.0;
	bool m_bPressed;
	bool m_drawArea;
	bool y_boundary = false;

	QMenu *saveMenu;
	//QImage Mat2QImage(cv::Mat &mat);
	//void createMeau();
	bool actived = false;
	void calcXY(int &x, int &y);

signals:
	//void sendRange(cv::Point sp, cv::Point ep);
	void sendPos(int x, int y);
	void moveWindow(QPoint loc);
	void maxWindow();
	void signal_savedata(QString at);
	void sendArea(int x1, int y1, int x2, int y2, int flag);

private slots:
	//void setImage(cv::Mat mat);
	/*void loadDefualtPNG();
	void actionsSlot();*/
};

