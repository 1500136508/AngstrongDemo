#pragma execution_character_set("utf-8")
#include <QDebug>
#include <QPoint>
#include <QFileDialog>
#include <highgui.hpp>
#include <imgproc.hpp>
#include "imageview.h"

ImageView::ImageView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);

	//��ʼ��
	m_spScene = nullptr;
	m_spItem = nullptr;
	m_spPix = nullptr;
	m_fScale = 1.0f;
	
	ui->m_gView_ImageView->setContextMenuPolicy(Qt::CustomContextMenu);//����Ϊ�Զ�������Ҽ��˵�ģʽ
	connect(ui->m_gView_ImageView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));//���ź�������Ӧ�Ĳۺ���

	//����ImageView�Ի������۴���
	setWindowFlags(Qt::FramelessWindowHint);

	//����QGraphicsView������Ԫ��
	m_spScene = std::make_shared<ImageScene>(new ImageScene());//���ӳ���
	m_spItem  = std::make_shared<ImageItem>(new ImageItem());//����ͼԪ
	m_spPix   = std::make_shared<ImagePix>(new ImagePix());//����ͼ��ͼԪ
	m_spScene->addItem(m_spPix.get());//�����ԪԪ�ذ󶨵�Scene
	ui->m_gView_ImageView->setScene(m_spScene.get());//��QGraphicsView���Scene;

	setMouseTracking(true);//������겶��
	ui->m_gView_ImageView->installEventFilter(this);//���öԻ������¼�����ضԻ����ϵĿؼ�

	//�����źŲ�
	BuildConnet();
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::Zoom(QPointF pointF, double fScale)
{
}

void ImageView::ZoomIn(QPointF poinF, double fScale)
{
}

void ImageView::ZoomOut(QPointF pointF, double fScale)
{
}

void ImageView::ZoomFit()
{
	if (!m_spScene || !m_spPix)
	{
		return;
	}
	
	QRect viewRect = ui->m_gView_ImageView->geometry();
	m_spScene->setSceneRect(0, 0, viewRect.width(), viewRect.height()); //������ԭ��������ʾ���ڵ����Ͻ�
	QPixmap pix = QPixmap::fromImage(qImage);
	m_spPix->setPixmap(pix.scaled(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio));
	//���þ�����ʾλ��
	int pWidth = m_spPix->pixmap().width();
	int pHeight = m_spPix->pixmap().height();
	int vWidth = viewRect.width();
	int vHeight = viewRect.height();
	if (vWidth > pWidth)
	{
		qreal fDeta_x = (vWidth - pWidth) / 2;
		m_spPix->setPos(fDeta_x, 0);
	} 
	else if(vHeight > pHeight)
	{
		qreal fDeta_y = (vHeight - pHeight) / 2;
		m_spPix->setPos(0, fDeta_y);
	}
}

bool ImageView::Open()
{
	bool bReturn = false;

	do
	{
		try
		{
			QString filename;
			filename = QFileDialog::getOpenFileName(this, tr("ѡ��ͼ��"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF *.tiff)"));
			if (filename.isEmpty())
			{
				break;
			}
			else
			{
				std::string str = filename.toStdString();  // ��filenameת��Ϊstring���ͣ�
				m_Image = cv::imread(str);

				qImage = cvMat2QImage(m_Image);
				m_spPix->setPixmap(QPixmap::fromImage(qImage));
				
				//������ʾλ��,Ĭ��Ϊͼ�������ʾ
				ZoomFit();
			}
		}
		catch (cv::Exception &e)
		{
			const char *err_msg = e.what();
			break;
		}

		bReturn = true;
	} while (false);

	return bReturn;
}

void ImageView::paintEvent(QPaintEvent * event)
{

	QDialog::paintEvent(event);
}

void ImageView::resizeEvent(QResizeEvent *event)
{
	ZoomFit();

	QDialog::resizeEvent(event);
}

bool ImageView::eventFilter(QObject * obj, QEvent * event)
{
	if (event->type() == QEvent::Wheel)
	{
		if (obj == ui->m_gView_ImageView && !m_spPix->pixmap().isNull())
		{
			return QDialog::eventFilter(obj, event);//���Ź��ܴ�ʱ�֣�Ŀǰ�ȷ��أ����������Ź���
			QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
			if ((wheelEvent->delta() > 0) && (m_fScale >= 50))//���Ŵ�ԭʼͼ���50��
			{
				return QDialog::eventFilter(obj, event);;
			}
			else if ((wheelEvent->delta() < 0) && (m_fScale <= 0.5))//ͼ����С������Ӧ��С֮��Ͳ�������С
			{
				//ResetItemPos();//����ͼƬ��С��λ�ã�ʹ֮����Ӧ�ؼ����ڴ�С
			}
			else
			{
				qreal qrealOriginScale = m_fScale;
				if (wheelEvent->delta() > 0)//��������ǰ����
				{
					m_fScale *= 1.1;//ÿ�ηŴ�10%
				}
				else
				{
					m_fScale *= 0.9;//ÿ����С10%
				}
				m_spPix->setScale(m_fScale);
				if (wheelEvent->delta() > 0)
				{
					m_spPix->moveBy(-wheelEvent->pos().x()*qrealOriginScale*0.1, -wheelEvent->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
				}
				else
				{
					m_spPix->moveBy(wheelEvent->pos().x()*qrealOriginScale*0.1, wheelEvent->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
				}
			}
		}
	}
	return QDialog::eventFilter(obj, event);
}

void ImageView::on_open_clicked()
{
	Open();
}

void ImageView::on_save_clicked()
{
	int a = 7;
}

void ImageView::on_close_clicked()
{
	m_camera.run(0);
}

void ImageView::on_zoomIn_clicked()
{
	int a = 7;
}

void ImageView::on_zoomOut_clicked()
{
	int a = 7;
}

void ImageView::on_zoomFit_clicked()
{
	ZoomFit();
}

void ImageView::on_measure_clicked()
{
	int a = 7;
}

void ImageView::contextMenuRequest(QPoint point)
{
	QAction *m_action[EImageViewMenu_Total];//��������
	QString m_strMenuName[EImageViewMenu_Total];
	QMenu *m_menu;//�����˵�
	QMenu *m_menu_child;
	std::vector<std::function<void()>> m_fvec;

	for (auto v : m_action)
	{
		v = nullptr;
	}
	//���ò˵�
	m_strMenuName[EImageViewMenu_Open] = "��";
	m_strMenuName[EImageViewMenu_Save] = "����";
	m_strMenuName[EImageViewMenu_Close] = "�ر�";
	m_strMenuName[EImageViewMenu_ZoomIn] = "�Ŵ�";
	m_strMenuName[EImageViewMenu_ZoomOut] = "��С";
	m_strMenuName[EImageViewMenu_ZoomFit] = "��Ӧ";
	m_strMenuName[EImageViewMenu_Measure] = "����";
	/*m_fvec.push_back(std::bind(&ImageView::on_open_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_save_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_close_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_zoomIn_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_zoomOut_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_zoomFit_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_measure_clicked, this));*/
	m_menu = new QMenu();
	m_menu_child = new QMenu();
	for (size_t i = EImageViewMenu_Open; i < EImageViewMenu_Total; ++i)
	{
		m_action[i] = new QAction(m_strMenuName[i]);
		m_menu->addAction(m_action[i]);
		//connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(m_fvec.at(i)()));//��Ч��������������
		if (i == EImageViewMenu_Close)
		{
			m_menu->addSeparator();
		}

		switch (i)
		{
		case EImageViewMenu_Open:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_open_clicked()));
		}
		break;
		case EImageViewMenu_Save:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_save_clicked()));
		}
		break;
		case EImageViewMenu_Close:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_close_clicked()));
		}
		break;
		case EImageViewMenu_ZoomIn:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_zoomIn_clicked()));
		}
		break;
		case EImageViewMenu_ZoomOut:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_zoomOut_clicked()));
		}
		break;
		case EImageViewMenu_ZoomFit:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_zoomFit_clicked()));
		}
		break;
		case EImageViewMenu_Measure:
		{
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_measure_clicked()));
		}
		break;
		default:
			break;
		}
	}
	m_menu->exec(QCursor::pos());

	if (m_menu)
	{
		delete m_menu;
		m_menu = nullptr;
	}
	if (m_menu_child)
	{
		delete m_menu_child;
		m_menu_child = nullptr;
	}
	for (auto v : m_action)
	{
		if (v)
		{
			delete v;
			v = nullptr;
		}
	}
}

void ImageView::SetImage(cv::Mat mat)
{
	if (m_spPix)
	{
		QImage qImage = cvMat2QImage(mat);
		if (!qImage.isNull())
		{
			m_spPix->setPixmap(QPixmap::fromImage(qImage));
		}
	}
}

void ImageView::cameraInitFlag(bool camStop)
{
	cameraReady = !camStop;
	if (camStop)
	{
		qDebug() << "����ͷδ����";
	} 
	else
	{
		qDebug() << "����ͷ������";
	}
}

void ImageView::showstatus(bool status)
{
	if (!cameraReady) return;
	if (status)
		qDebug() << "Ԥ����";
	else
		qDebug()<<"Ԥ����ֹ";
}

void ImageView::BuildConnet()
{
	//�źŲ۵Ĳ������Զ���ģ���ʱ��Ҫ��qRegisterMetaTypeע��һ����������
	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::string>("std::string");

	connect(&m_camera, SIGNAL(sendImage(cv::Mat)), this, SLOT(SetImage(cv::Mat)));
	connect(this, SIGNAL(closeEvent()), &m_camera, SLOT(stopingProgram()));
}

QImage ImageView::cvMat2QImage(const cv::Mat & mat)
{
	/*if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}*/

	if (mat.type() == CV_8UC1 || mat.type() == CV_32FC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}

cv::Mat ImageView::QImage2cvMat(QImage image)
{
	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}
