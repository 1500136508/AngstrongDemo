#pragma execution_character_set("utf-8")
#include <QDebug>
#include <QPoint>
#include <QFileDialog>
#include <QTimer>
#include <highgui.hpp>
#include <imgproc.hpp>
#include "imageview.h"

ImageView::ImageView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);

	//进行ImageView对话框的外观处理
	//setWindowFlags(Qt::FramelessWindowHint);

	setMouseTracking(true);//启动鼠标捕获

	//建立信号槽
	m_pCamera = nullptr;
	m_pCamera = new imageReader();
	BuildConnet();
	//qss 界面美化
}

ImageView::~ImageView()
{
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	setAttribute(Qt::WA_DeleteOnClose);
    delete ui;
}

void ImageView::paintEvent(QPaintEvent * event)
{

	QDialog::paintEvent(event);
}

void ImageView::resizeEvent(QResizeEvent *event)
{
	ui->m_gView_ImageView->ZoomFit();

	QDialog::resizeEvent(event);
}

bool ImageView::eventFilter(QObject * obj, QEvent * event)
{
	//if (event->type() == QEvent::Wheel)
	//{
	//	if (obj == ui->m_gView_ImageView && !m_spPix->pixmap().isNull())
	//	{
	//		return QDialog::eventFilter(obj, event);//缩放功能待时现，目前先返回，不启用缩放功能
	//		QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
	//		if ((wheelEvent->delta() > 0) && (m_fScale >= 50))//最大放大到原始图像的50倍
	//		{
	//			return QDialog::eventFilter(obj, event);;
	//		}
	//		else if ((wheelEvent->delta() < 0) && (m_fScale <= 0.5))//图像缩小到自适应大小之后就不继续缩小
	//		{
	//			//ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
	//		}
	//		else
	//		{
	//			qreal qrealOriginScale = m_fScale;
	//			if (wheelEvent->delta() > 0)//鼠标滚轮向前滚动
	//			{
	//				m_fScale *= 1.1;//每次放大10%
	//			}
	//			else
	//			{
	//				m_fScale *= 0.9;//每次缩小10%
	//			}
	//			m_spPix->setScale(m_fScale);
	//			if (wheelEvent->delta() > 0)
	//			{
	//				m_spPix->moveBy(-wheelEvent->pos().x()*qrealOriginScale*0.1, -wheelEvent->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
	//			}
	//			else
	//			{
	//				m_spPix->moveBy(wheelEvent->pos().x()*qrealOriginScale*0.1, wheelEvent->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
	//			}
	//		}
	//	}
	//}
	return QDialog::eventFilter(obj, event);
}

void ImageView::closeEvent(QCloseEvent * event)
{
}

void ImageView::BuildConnet()
{
	//信号槽的参数是自定义的，这时需要用qRegisterMetaType注册一下这种类型
	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::string>("std::string");

	connect(m_pCamera, SIGNAL(sendImage(cv::Mat)), ui->m_gView_ImageView, SLOT(SetImage(cv::Mat)));
	connect(this, SIGNAL(closeEvent()), m_pCamera, SLOT(stopingProgram()));
}
