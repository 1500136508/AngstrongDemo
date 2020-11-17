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

	//��ʼ��
	m_pCamera				= nullptr;
	m_qstrSavePath			= ("./data");//Ĭ��·��
	m_bIsSaveRGB			= false;
	m_bIsSaveDepth			= false;
	m_bIsSaveIR				= false;
	m_bIsSavePointCloud		= false;
	m_nSaveDataIndex        = 0;
	for (int i = 0; i< BUFFSIZE;++i)
	{
		m_pDataDepth[i] = nullptr;
	}

	//����ImageView�Ի������۴���
	//setWindowFlags(Qt::FramelessWindowHint);
	setMouseTracking(true);//������겶��

	//�źŲ۵Ĳ������Զ���ģ���ʱ��Ҫ��qRegisterMetaTypeע��һ����������
	qRegisterMetaType<cv::Mat>("cv::Mat");
	//qRegisterMetaType<std::string>("std::string");
	BuildConnet();//�����źŲ�
	//qss ��������
	QFile file("../qss/black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	setStyleSheet(stylesheet);
	ui->statusWidget->setStyleSheet(stylesheet);
	ui->titleWidget->setStyleSheet(stylesheet);

	//Only test
	m_pCamera = new imageReader();
	if (m_pCamera)
	{
		m_pCamera->run(0);
	}
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

void ImageView::ReceiveImageInfo(bool bIsEmpty, int nImageWidth, int nImageHeight)
{
	if (bIsEmpty)
	{
		QString qstrImageInfo("");
		qstrImageInfo.append("[");
		qstrImageInfo.append(QString::number(nImageWidth));
		qstrImageInfo.append(" x ");
		qstrImageInfo.append(QString::number(nImageHeight));
		qstrImageInfo.append("]");
		ui->m_lab_ImageInfo->setText(qstrImageInfo);
	}
	else
	{
		ui->m_lab_ImageInfo->setText("[No Image]");
	}
}

void ImageView::ReceiveMouseInfo(int x, int y)
{
	if (x < 0 || y < 0)
	{
		ui->m_lab_xy->setText("(out of range)");
	}
	else
	{
		QString qstrMouseInfo("");
		qstrMouseInfo.append("(");
		qstrMouseInfo.append(QString::number(x));
		qstrMouseInfo.append(",");
		qstrMouseInfo.append(QString::number(y));
		qstrMouseInfo.append(")");
		ui->m_lab_xy->setText(qstrMouseInfo);
	}
}

void ImageView::ReceiveImageGray(int R,int G,int B)
{
	QString qstrImageGray("");
	qstrImageGray.append("(");
	qstrImageGray.append(QString::number(R));
	qstrImageGray.append(",");
	qstrImageGray.append(QString::number(G));
	qstrImageGray.append(",");
	qstrImageGray.append(QString::number(B));
	qstrImageGray.append(")");
	ui->m_lab_RGB->setText(qstrImageGray);
}

void ImageView::ReceiveSaveImageData(cv::Mat ImageIR, cv::Mat ImageRGB, float * depth)
{
	ImageIR.copyTo(m_cvImageIR[m_nSaveDataIndex]);
	ImageRGB.copyTo(m_cvImageRGB[m_nSaveDataIndex]);
#ifndef KEEP_ORI
	memcpy(m_pDataDepth[m_nSaveDataIndex], depth, 848 * 480 * sizeof(float));
#else
	memcpy(m_pDataDepth[m_nSaveDataIndex], depth, 640 * 400 * sizeof(float));
#endif
	++m_nSaveDataIndex;
	if (m_nSaveDataIndex == BUFFSIZE)
	{
		m_nSaveDataIndex = 0;
	}
}

void ImageView::ReceiveSaveDataStatus(bool bSave, int eMode, int nSaveCount)
{
	m_pCamera->SetSaveImageStatus(bSave);
}

//void ImageView::ReceiveImageRGB(int nsize, uchar * data, cv::Mat mat)
//{
//	//Ŀǰ��ԭʼ����data������
//	m_cvImageRGB = mat;
//	static int sIndexRGB = 0;
//	if (m_bIsSaveRGB)
//	{
//		++sIndexRGB;
//		QString qstrNameExtra("");
//		if (sIndexRGB < 10)
//		{
//			qstrNameExtra = "000";
//		}
//		else if (sIndexRGB < 100)
//		{
//			qstrNameExtra = "00";
//		}
//		else if (sIndexRGB < 1000)
//		{
//			qstrNameExtra = "0";
//		}
//		m_qstrSavePath = m_qstrSavePath + "//rgb//"+ qstrNameExtra + QString::number(sIndexRGB) + ".jpg";
//		cv::imwrite(m_qstrSavePath.toStdString(), m_cvImageRGB);
//	}
//	else
//	{
//		//++sIndexRGB = -1;
//	}
//}
//
//void ImageView::ReceiveImageDepth(int nsize, float * data, cv::Mat mat)
//{
//	//Ŀǰ��ԭʼ����data������
//	m_cvImageDepth = mat;
//	static int sIndexDepth = 0;
//	if (m_bIsSaveDepth)
//	{
//		++sIndexDepth;
//		QString qstrNameExtra("");
//		if (sIndexDepth < 10)
//		{
//			qstrNameExtra = "000";
//		}
//		else if(sIndexDepth < 100)
//		{
//			qstrNameExtra = "00";
//		}
//		else if(sIndexDepth < 1000)
//		{
//			qstrNameExtra = "0";
//		}
//		m_qstrSavePath = m_qstrSavePath + "//depth//" + qstrNameExtra + QString::number(sIndexDepth) + ".png";
//		cv::imwrite(m_qstrSavePath.toStdString(), m_cvImageRGB);
//	}
//	else
//	{
//		
//	}
//}
//
//void ImageView::ReceiveImageIR(int nsize, unsigned char * data, cv::Mat mat)
//{
//	//Ŀǰ��ԭʼ����data������
//	m_cvImageIR = mat;
//	static int sIndexIR = 0;
//	if (m_bIsSaveIR)
//	{
//		++sIndexIR;
//		QString qstrNameExtra("");
//		if (sIndexIR < 10)
//		{
//			qstrNameExtra = "000";
//		}
//		else if (sIndexIR < 100)
//		{
//			qstrNameExtra = "00";
//		}
//		else if (sIndexIR < 1000)
//		{
//			qstrNameExtra = "0";
//		}
//		m_qstrSavePath = m_qstrSavePath + "//ir//"+qstrNameExtra + QString::number(sIndexIR) + ".png";
//		cv::imwrite(m_qstrSavePath.toStdString(), m_cvImageRGB);
//	}
//	else
//	{
//		
//	}
//}

void ImageView::BuildConnet()
{
	connect(m_pCamera, SIGNAL(sendImage(cv::Mat)), ui->m_gView_ImageView, SLOT(SetImage(cv::Mat)));
	connect(ui->m_gView_ImageView, SIGNAL(SendImageInfo(bool, int, int)), this, SLOT(ReceiveImageInfo(bool, int, int)));//����ͼ����Ϣ
	connect(ui->m_gView_ImageView, SIGNAL(SendMouseInfo(int, int)), this, SLOT(ReceiveMouseInfo(int, int)));//���������ͼ���ϵ�λ����Ϣ
	connect(ui->m_gView_ImageView, SIGNAL(SendImageGray(int,int,int)), this, SLOT(ReceiveImageGray(int, int, int)));//��������Ӧ��ͼ�����ػҶ�ֵ��Ϣ
	connect(m_pCamera, SIGNAL(sendSaveImageData(cv::Mat,cv::Mat,float*)), this, SLOT(ReceiveSaveImageData(cv::Mat, cv::Mat, float*)));
}
