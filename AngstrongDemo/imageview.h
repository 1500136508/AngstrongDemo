#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QDialog>
#include <core.hpp>
#include <memory>
#include <functional>
#include <vector>
#include <QString>
#include <QMenu>
#include <QAction>
#include "ui_imageview.h"
#include "imagescene.h"
#include "imageitem.h"
#include "imagepix.h"
#include "definitionmenu.h"
#include "imagereader.h"

namespace Ui {
class ImageView;
}

class ImageView : public QDialog
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

	void Zoom(QPointF pointF, double fScale = 1.0f);
	void ZoomIn(QPointF poinF, double fScale = 1.2f);
	void ZoomOut(QPointF pointF, double fScale = 0.8f);
	void ZoomFit();
protected:
	bool Open();//Only Framework

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *);
	bool eventFilter(QObject *obj, QEvent *event);
private slots:
	void on_open_clicked();
	void on_save_clicked();
	void on_close_clicked();
	void on_zoomIn_clicked();
	void on_zoomOut_clicked();
	void on_zoomFit_clicked();
	void on_measure_clicked();
	void contextMenuRequest(QPoint);

	void SetImage(cv::Mat mat);//����ͼ��ר�òۺ���
	void cameraInitFlag(bool camStop);// ��ʾ����ͷ״̬
	void showstatus(bool status);
signals:
	void sendCloseEven();
public:
    Ui::ImageView *ui;

	//����ɼ�ͼ�������
	imageReader *m_pCamera;
private:
	void BuildConnet();
	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);

	std::shared_ptr<ImageScene> m_spScene;
	std::shared_ptr<ImageItem>  m_spItem;
	std::shared_ptr<ImagePix>   m_spPix;

	//��ͼ����صı���
	cv::Mat m_Image;
	QImage qImage;
	double m_fScale;

	bool cameraReady = false;
};

#endif // IMAGEVIEW_H
