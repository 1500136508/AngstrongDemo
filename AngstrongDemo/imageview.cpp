#pragma execution_character_set("utf-8")
#include <QDebug>
#include <QPoint>
#include <QFileDialog>
#include <QTimer>
#include <highgui.hpp>
#include <imgproc.hpp>
#include "imageview.h"
#include "savedata.h"

ImageView::ImageView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);

	//初始化
	m_pCamera				= nullptr;
	m_qstrSavePath			= ("./data");//默认路径
	m_bIsSaveRGB			= false;
	m_bIsSaveDepth			= false;
	m_bIsSaveIR				= false;
	m_bIsSavePointCloud		= false;
	m_nSaveDataIndex        = 0;
	m_nSaveImageCount		= 0;
	m_nWriteIndex			= 0;
	m_bSaveFinish			= false;
	m_bProgramExit			= false;
	m_nMode					= 3;//默认为三图模式
	for (int i = 0; i< BUFFSIZE;++i)
	{
		m_pDataDepth[i] = new float[848 * 480];
	}
	m_pCamera = new imageReader();
	//m_pThreadSaveImage = new std::thread(&ImageView::SaveImageThread,this);//启动保存图片线程
	//m_pThreadSaveImage->detach();

	//进行ImageView对话框的外观处理
	//setWindowFlags(Qt::FramelessWindowHint);
	setMouseTracking(true);//启动鼠标捕获

	//信号槽的参数是自定义的，这时需要用qRegisterMetaType注册一下这种类型
	qRegisterMetaType<cv::Mat>("cv::Mat");
	//qRegisterMetaType<std::string>("std::string");
	BuildConnet();//建立信号槽
	//qss 界面美化
	QFile file("../qss/black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	setStyleSheet(stylesheet);
	ui->statusWidget->setStyleSheet(stylesheet);
	ui->titleWidget->setStyleSheet(stylesheet);
}

ImageView::~ImageView()
{
	m_bProgramExit = true;
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	for (int i = 0; i < BUFFSIZE; ++i)
	{
		delete[] m_pDataDepth[i];
		m_pDataDepth[i] = nullptr;
	}
	/*if (m_pThreadSaveImage)
	{
		delete m_pThreadSaveImage;
		m_pThreadSaveImage = nullptr;
	}*/

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
	if (m_nSaveDataIndex == BUFFSIZE)
	{
		return;
	}
	ImageIR.copyTo(m_cvImageIR[m_nSaveDataIndex]);
	ImageRGB.copyTo(m_cvImageRGB[m_nSaveDataIndex]);
#ifndef KEEP_ORI
	memcpy(m_pDataDepth[m_nSaveDataIndex], depth, 848 * 480 * sizeof(float));
#else
	memcpy(m_pDataDepth[m_nSaveDataIndex], depth, 640 * 400 * sizeof(float));
#endif
	++m_nSaveDataIndex;
}

void ImageView::ReceiveSaveDataStatus(bool bSave, int eMode, int nSaveCount, QString qstrPath)
{
	m_qstrSavePath = qstrPath;
	m_pCamera->SetSaveImageStatus(bSave);
	if (!bSave)
	{
		m_nSaveDataIndex = 0;//数据清零
	}
	m_nMode = eMode;
	m_nSaveImageCount = nSaveCount;
}

void ImageView::ReceiveCameraStatus(ECameraStatus eStatus)
{
	//对接收到的相机状态进行处理
	switch (eStatus)
	{
	case ECameraStatus_Unknow:
	{

	}
		break;
	case ECameraStatus_Open:
	{
		if (m_pCamera)
		{
			//m_pCamera->OpenCamera(0);
			m_pCamera->run(0);
			emit SendCameraStatus(ECameraStatus_Open);
		}
	}
		break;
	case ECameraStatus_Close:
	{
		if (m_pCamera)
		{
			m_pCamera->CloseCamera();
			emit SendCameraStatus(ECameraStatus_Close);
		}
	}
		break;
	case ECameraStatus_Live:
	{
		if (m_pCamera)
		{
			m_pCamera->Live();
			emit SendCameraStatus(ECameraStatus_Live);
		}
	}
		break;
	case ECameraStatus_Pause:
	{
		if (m_pCamera)
		{
			m_pCamera->Pause();
			emit SendCameraStatus(ECameraStatus_Pause);
		}
	}
		break;
	case ECameraStatus_Stop:
	{
		if (m_pCamera)
		{
			m_pCamera->Stop();
			emit SendCameraStatus(ECameraStatus_Stop);
		}
	}
		break;
	default:
		break;
	}
}

//void ImageView::ReceiveImageRGB(int nsize, uchar * data, cv::Mat mat)
//{
//	//目前对原始数据data不处理
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
//	//目前对原始数据data不处理
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
//	//目前对原始数据data不处理
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
	bool ret = connect(m_pCamera, SIGNAL(sendImage(cv::Mat)), ui->m_gView_ImageView, SLOT(SetImage(cv::Mat)));
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendImageInfo(bool, int, int)), this, SLOT(ReceiveImageInfo(bool, int, int)));//接收图像信息
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendMouseInfo(int, int)), this, SLOT(ReceiveMouseInfo(int, int)));//接收鼠标在图像上的位置信息
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendImageGray(int,int,int)), this, SLOT(ReceiveImageGray(int, int, int)));//接收鼠标对应的图像像素灰度值信息
	ret = connect(m_pCamera, SIGNAL(sendSaveImageData(cv::Mat,cv::Mat,float*)), this, SLOT(ReceiveSaveImageData(cv::Mat, cv::Mat, float*)));
}

void ImageView::SaveImageThread()
{
	int frameHeightR = 480;
	int frameWidthR = 848;
	int frameHeight = 400;
	int frameWidth = 640;
	FILE *plyfile;

	while (!m_bProgramExit)
	{
		if (m_nWriteIndex < m_nSaveDataIndex)
		{
			if (m_nWriteIndex >= m_nSaveImageCount)
			{
				m_bSaveFinish = true;
				m_nWriteIndex = 0;
				m_nSaveImageCount = 0;
				continue;
			}
			//开始保存图片............
			QString qstrNameExtra("");
			if (m_nWriteIndex < 10)
			{
				qstrNameExtra = "000";
			}
			else if (m_nWriteIndex < 100)
			{
				qstrNameExtra = "00";
			}
			else if (m_nWriteIndex < 1000)
			{
				qstrNameExtra = "0";
			}

			//先保存IR图
			QString qstrSavePath_IR = m_qstrSavePath + "//ir//" + qstrNameExtra + QString::number(m_nWriteIndex) + ".png";
			cv::cvtColor(m_cvImageIR[m_nWriteIndex], m_cvImageIR[m_nWriteIndex], cv::COLOR_RGB2GRAY);
			cv::imwrite(qstrSavePath_IR.toStdString(), m_cvImageIR[m_nWriteIndex]);

			//保存深度图
			QString qstrSavePath_Depth = m_qstrSavePath + "//depth//" + qstrNameExtra + QString::number(m_nWriteIndex) + ".png";
#ifndef KEEP_ORI
			cv::Mat saveMat = cv::Mat(cv::Size(frameHeightR, frameWidthR), CV_16UC1);
			for (int x = 0; x < frameHeightR; x++) {
				for (int y = 0; y < frameWidthR; y++) {
					saveMat.at<short>(y, x) = short(m_pDataDepth[m_nWriteIndex][y*frameHeightR + x]);
				}
			}
#else
			cv::Mat saveMat = cv::Mat(cv::Size(frameHeight, frameWidth), CV_16UC1);
			for (int x = 0; x < frameHeight; x++) {
				for (int y = 0; y < frameWidth; y++) {
					saveMat.at<short>(y, x) = short(m_pDataDepth[m_nWriteIndex][y*frameHeight + x]);
				}
			}
#endif
			cv::imwrite(qstrSavePath_Depth.toStdString(), saveMat);
			//保存RGB图
			QString qstrSavePath = m_qstrSavePath + "//rgb//" + qstrNameExtra + QString::number(m_nWriteIndex) + ".jpg";
			cv::imwrite(qstrSavePath.toStdString(), m_cvImageRGB[m_nWriteIndex]);

			switch (m_nMode)
			{
			case SaveData::ESaveMode_3Pix:
			{
				//doing something.........
			}
				break;
			case SaveData::ESaveMode_4Pix:
			{
				//保存点云图
				QString qstrSavePath_CloudPoint = m_qstrSavePath + "//pointcloud//" + qstrNameExtra + QString::number(m_nWriteIndex) + ".txt";
				fopen_s(&plyfile, qstrSavePath_CloudPoint.toStdString().c_str(), "w");
				for (int x = 0; x < frameHeightR; x++) {
					for (int y = 0; y < frameWidthR; y++) {
						float z = m_pDataDepth[m_nWriteIndex][y*frameHeightR + x];
						if (z < 10e-6 && z > -10e-6) continue;
						float x_rw = ((float)x - m_pCamera->dsaver->cx) * z / m_pCamera->dsaver->fx;
						float y_rw = ((float)y - m_pCamera->dsaver->cy) * z / m_pCamera->dsaver->fy;
						fprintf(plyfile, "%.6f %.6f %.6f\n", x_rw, y_rw, z);
					}
				}
				fclose(plyfile);
			}
				break;
			default:
				break;
			}
			++m_nWriteIndex;
		}
	}
}
