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

	//��ʼ��
	m_pCamera				= nullptr;
	m_qstrSavePath			= ("./data");//Ĭ��·��
	m_bIsSaveRGB			= false;
	m_bIsSaveDepth			= false;
	m_bIsSaveIR				= false;
	m_bIsSavePointCloud		= false;
	m_nSaveDataIndex        = 0;
	m_nSaveImageCount		= 0;
	m_nWriteIndex			= 0;
	m_bSaveFinish			= false;
	m_bProgramExit			= false;
	m_nMode					= 3;//Ĭ��Ϊ��ͼģʽ
	for (int i = 0; i< BUFFSIZE;++i)
	{
		m_pDataDepth[i] = new float[848 * 480];
	}
	m_pCamera = new imageReader();
	m_pThreadSaveImage = new std::thread(&ImageView::SaveImageThread,this);//��������ͼƬ�߳�
	m_pThreadSaveImage->detach();

	//����ImageView�Ի������۴���
	//setWindowFlags(Qt::FramelessWindowHint);
	setMouseTracking(true);//������겶��

	//�źŲ۵Ĳ������Զ���ģ���ʱ��Ҫ��qRegisterMetaTypeע��һ����������
	qRegisterMetaType<cv::Mat>("cv::Mat");
	//qRegisterMetaType<std::string>("std::string");
	BuildConnet();//�����źŲ�
	//qss ��������
	QFile file("black.qss");
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
	if (m_pThreadSaveImage)
	{
		delete m_pThreadSaveImage;
		m_pThreadSaveImage = nullptr;
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
	if (!m_pCamera->IsRunning())
	{
		emit SendSaveImageInfo("No Running Now...");
		return;
	}
	
	m_pCamera->SetSaveImageStatus(bSave);
	m_bIsSaveImage = bSave;
	if (!bSave)
	{
		m_pCamera->SetSaveImageStatus(false);
		m_bSaveFinish = true;
		m_nWriteIndex = 0;
		m_nSaveImageCount = 0;
		m_nSaveDataIndex = 0;
		Sleep(20);
		emit SendSaveImageInfo("Stop to save!");
		return;
	}
	m_qstrSavePath = qstrPath;
	m_nMode = eMode;
	m_nSaveImageCount = nSaveCount;
	m_bSaveFinish = false;
	QString qstrSaveImageInfo = "Star to Save: " + QString::number(0) + "/" + QString::number(m_nSaveImageCount);
	emit SendSaveImageInfo(qstrSaveImageInfo);
}

void ImageView::ReceiveCameraStatus(ECameraStatus eStatus)
{
	//�Խ��յ������״̬���д���
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
			if (m_pCamera->OpenCamera(0))
			{
				emit SendCameraStatus(ECameraStatus_Open);
			}
		}
	}
		break;
	case ECameraStatus_Close:
	{
		if (m_pCamera)
		{
			m_pCamera->CloseCamera();
			emit SendCameraStatus(ECameraStatus_Close);
			//ui->m_gView_ImageView->ClearAll();
		}
	}
		break;
	case ECameraStatus_Live:
	{
		if (m_pCamera)
		{
			if (m_pCamera->IsOpen())
			{
				m_pCamera->Live();
				emit SendCameraStatus(ECameraStatus_Live);
			}
		}
	}
		break;
	case ECameraStatus_Pause:
	{
		if (m_pCamera)
		{
			if (m_pCamera->IsRunning())
			{
				m_pCamera->Pause();
				emit SendCameraStatus(ECameraStatus_Pause);
			}
		}
	}
		break;
	case ECameraStatus_Stop:
	{
		if (m_pCamera)
		{
			if (m_pCamera->IsRunning())
			{
				m_pCamera->Stop();
			}
			emit SendCameraStatus(ECameraStatus_Stop);
		}
	}
		break;
	default:
		break;
	}
}

void ImageView::BuildConnet()
{
	bool ret = connect(m_pCamera, SIGNAL(sendImage(cv::Mat)), ui->m_gView_ImageView, SLOT(SetImage(cv::Mat)));
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendImageInfo(bool, int, int)), this, SLOT(ReceiveImageInfo(bool, int, int)));//����ͼ����Ϣ
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendMouseInfo(int, int)), this, SLOT(ReceiveMouseInfo(int, int)));//���������ͼ���ϵ�λ����Ϣ
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendImageGray(int,int,int)), this, SLOT(ReceiveImageGray(int, int, int)));//��������Ӧ��ͼ�����ػҶ�ֵ��Ϣ
	ret = connect(m_pCamera, SIGNAL(sendSaveImageData(cv::Mat,cv::Mat,float*)), this, SLOT(ReceiveSaveImageData(cv::Mat, cv::Mat, float*)));
	ret = connect(ui->m_gView_ImageView, SIGNAL(SendMouseInfo(int, int)), m_pCamera, SLOT(ReceiveMouseInfo(int, int)));//���������ͼ���ϵ�λ����Ϣ
	connect(ui->m_gView_ImageView, SIGNAL(SendAvgArea(int, QRectF)), m_pCamera, SLOT(ReceiveAvgArea(int, QRectF)));
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
		if (!m_bIsSaveImage)
		{
			Sleep(3);
			continue;
		}
		if (m_nWriteIndex < m_nSaveDataIndex)
		{
			if (!m_bSaveFinish && m_nWriteIndex >= m_nSaveImageCount)
			{
				m_pCamera->SetSaveImageStatus(false);
				m_bSaveFinish = true;
				m_nWriteIndex = 0;
				m_nSaveImageCount = 0;
				m_nSaveDataIndex = 0;
				emit SendSaveImageInfo("Save to Done");
				continue;
			}
			if (m_bSaveFinish)
			{
				Sleep(3);
				continue;
			}
			//��ʼ����ͼƬ............
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
			
			try
			{
				//�ȱ���IRͼ
				QString qstrSavePath_IR = m_qstrSavePath + "//ir//" + qstrNameExtra + QString::number(m_nWriteIndex) + ".png";
				cv::cvtColor(m_cvImageIR[m_nWriteIndex], m_cvImageIR[m_nWriteIndex], cv::COLOR_RGB2GRAY);
				cv::imwrite(qstrSavePath_IR.toStdString(), m_cvImageIR[m_nWriteIndex]);
				//�������ͼ
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
				//����RGBͼ
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
					//�������ͼ
					QString qstrSavePath_CloudPoint = m_qstrSavePath + "//pointcloud//" + qstrNameExtra + QString::number(m_nWriteIndex) + ".txt";
					fopen_s(&plyfile, qstrSavePath_CloudPoint.toStdString().c_str(), "w");
					for (int x = 0; x < frameHeightR; x++)
					{
						for (int y = 0; y < frameWidthR; y++)
						{
							float z = m_pDataDepth[m_nWriteIndex][y*frameHeightR + x];
							if (z < 10e-6 && z > -10e-6) continue;
							float x_rw = ((float)x - m_pCamera->cx) * z / m_pCamera->fx;
							float y_rw = ((float)y - m_pCamera->cy) * z / m_pCamera->fy;
							fprintf(plyfile, "%.6f %.6f %.6f\n", x_rw, y_rw, z);
						}
					}
					fclose(plyfile);
				}
				break;
				default:
					break;
				}
				QString qstrSaveImageInfo = "Star to Save: " + QString::number(m_nWriteIndex + 1) + "/" + QString::number(m_nSaveImageCount);
				emit SendSaveImageInfo(qstrSaveImageInfo);
				++m_nWriteIndex;
			}
			catch (cv::Exception &e)
			{
				std::string strErr = e.what();
				Sleep(3);
				m_bSaveFinish = true;
				emit SendSaveImageInfo("Error:Fail to save!!!" );
				continue;
			}
		}
		else
		{
			Sleep(3);
			continue;
		}
	}
}
