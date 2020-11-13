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

	//����ImageView�Ի������۴���
	//setWindowFlags(Qt::FramelessWindowHint);

	setMouseTracking(true);//������겶��

	//�����źŲ�
	m_pCamera = nullptr;
	m_pCamera = new imageReader();
	BuildConnet();
	//qss ��������
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
	//		return QDialog::eventFilter(obj, event);//���Ź��ܴ�ʱ�֣�Ŀǰ�ȷ��أ����������Ź���
	//		QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
	//		if ((wheelEvent->delta() > 0) && (m_fScale >= 50))//���Ŵ�ԭʼͼ���50��
	//		{
	//			return QDialog::eventFilter(obj, event);;
	//		}
	//		else if ((wheelEvent->delta() < 0) && (m_fScale <= 0.5))//ͼ����С������Ӧ��С֮��Ͳ�������С
	//		{
	//			//ResetItemPos();//����ͼƬ��С��λ�ã�ʹ֮����Ӧ�ؼ����ڴ�С
	//		}
	//		else
	//		{
	//			qreal qrealOriginScale = m_fScale;
	//			if (wheelEvent->delta() > 0)//��������ǰ����
	//			{
	//				m_fScale *= 1.1;//ÿ�ηŴ�10%
	//			}
	//			else
	//			{
	//				m_fScale *= 0.9;//ÿ����С10%
	//			}
	//			m_spPix->setScale(m_fScale);
	//			if (wheelEvent->delta() > 0)
	//			{
	//				m_spPix->moveBy(-wheelEvent->pos().x()*qrealOriginScale*0.1, -wheelEvent->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
	//			}
	//			else
	//			{
	//				m_spPix->moveBy(wheelEvent->pos().x()*qrealOriginScale*0.1, wheelEvent->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
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
	//�źŲ۵Ĳ������Զ���ģ���ʱ��Ҫ��qRegisterMetaTypeע��һ����������
	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::string>("std::string");

	connect(m_pCamera, SIGNAL(sendImage(cv::Mat)), ui->m_gView_ImageView, SLOT(SetImage(cv::Mat)));
	connect(this, SIGNAL(closeEvent()), m_pCamera, SLOT(stopingProgram()));
}
