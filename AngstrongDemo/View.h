#pragma once
#include <QGraphicsView>
#include <vector>
#include "imagescene.h"
#include "imageitem.h"
#include "imagepix.h"
#include "graphicsrectitem.h"
#include "definitionmenu.h"

class View : public QGraphicsView
{
	Q_OBJECT
public:
	View(QWidget *pParent = nullptr);
	View(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~View();

	int GetImageWidth()const;
	int GetImageHeight()const;
	void Zoom(QPointF pointF, double fScale = 1.0f);
	void ZoomIn(QPointF poinF, double fScale = 1.2f);
	void ZoomOut(QPointF pointF, double fScale = 0.8f);
	void ZoomFit();

	std::shared_ptr<ImageScene> GetScene()const;
	void ClearAll();

protected:
	bool Open();//Only Framework
	bool Save();//Only Framework

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent*event);

	void paintEvent(QPaintEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
private slots:

	void on_open_clicked();
	void on_save_clicked();
	void on_close_clicked();
	void on_zoomIn_clicked();
	void on_zoomOut_clicked();
	void on_zoomFit_clicked();
	void on_measure_clicked();
	void on_measureRect_clicked();
	void on_measureCircle_clicked();

	void SetImage(cv::Mat mat);//����ͼ��ר�òۺ���

	void ReceiveCreateAvgArea(int nIndex,bool bIsCreate);//����avgArea����Ϣ
	void CalcInfo(QPointF point);
	void CalcAvgArea();
signals:
	void SendImageInfo(bool bIsEmpty,int nImageWidth,int nImageHeight);
	void SendMouseInfo(int x, int y);
	void SendImageGray(int R,int G,int B);
	void SendAvgArea(int nIndex, QRectF rect);
private:
	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);

private:
	std::shared_ptr<ImageScene>			m_spScene;
	std::shared_ptr<ImageItem>			m_spItem;
	std::shared_ptr<ImagePix>			m_spPix;
	std::shared_ptr<GraphicsRectItem>   m_spRect;
	volatile bool first_time_to_live_;

	std::vector<std::shared_ptr<GraphicsRectItem>>   m_vecAvgROI;//���ڴ洢AvgROI�������Ϣ
	//��ͼ����صı���
	cv::Mat m_Image;
	QImage qImage;
	int m_ImageWidth;
	int m_ImageHeight;

	QString stylesheet;//qss
};

