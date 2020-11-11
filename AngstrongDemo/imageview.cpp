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

	//初始化
	m_spScene = nullptr;
	m_spItem = nullptr;
	m_spPix = nullptr;
	m_fScale = 1.0f;
	
	ui->m_gView_ImageView->setContextMenuPolicy(Qt::CustomContextMenu);//设置为自定义鼠标右键菜单模式
	connect(ui->m_gView_ImageView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));//给信号设置相应的槽函数

	//进行ImageView对话框的外观处理
	setWindowFlags(Qt::FramelessWindowHint);

	//增加QGraphicsView框架相关元素
	m_spScene = std::make_shared<ImageScene>(new ImageScene());//增加场景
	m_spItem  = std::make_shared<ImageItem>(new ImageItem());//增肌图元
	m_spPix   = std::make_shared<ImagePix>(new ImagePix());//增加图像图元
	m_spScene->addItem(m_spPix.get());//添加像元元素绑定到Scene
	ui->m_gView_ImageView->setScene(m_spScene.get());//将QGraphicsView添加Scene;

	setMouseTracking(true);//启动鼠标捕获
	ui->m_gView_ImageView->installEventFilter(this);//设置对话框监控事件，监控对话框上的控件

	//建立信号槽
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
	m_spScene->setSceneRect(0, 0, viewRect.width(), viewRect.height()); //将坐标原点设在显示窗口的左上角
	QPixmap pix = QPixmap::fromImage(qImage);
	m_spPix->setPixmap(pix.scaled(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio));
	//设置居中显示位置
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
			filename = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF *.tiff)"));
			if (filename.isEmpty())
			{
				break;
			}
			else
			{
				std::string str = filename.toStdString();  // 将filename转变为string类型；
				m_Image = cv::imread(str);

				qImage = cvMat2QImage(m_Image);
				m_spPix->setPixmap(QPixmap::fromImage(qImage));
				
				//处理显示位置,默认为图像居中显示
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
			return QDialog::eventFilter(obj, event);//缩放功能待时现，目前先返回，不启用缩放功能
			QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
			if ((wheelEvent->delta() > 0) && (m_fScale >= 50))//最大放大到原始图像的50倍
			{
				return QDialog::eventFilter(obj, event);;
			}
			else if ((wheelEvent->delta() < 0) && (m_fScale <= 0.5))//图像缩小到自适应大小之后就不继续缩小
			{
				//ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
			}
			else
			{
				qreal qrealOriginScale = m_fScale;
				if (wheelEvent->delta() > 0)//鼠标滚轮向前滚动
				{
					m_fScale *= 1.1;//每次放大10%
				}
				else
				{
					m_fScale *= 0.9;//每次缩小10%
				}
				m_spPix->setScale(m_fScale);
				if (wheelEvent->delta() > 0)
				{
					m_spPix->moveBy(-wheelEvent->pos().x()*qrealOriginScale*0.1, -wheelEvent->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
				}
				else
				{
					m_spPix->moveBy(wheelEvent->pos().x()*qrealOriginScale*0.1, wheelEvent->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
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
	QAction *m_action[EImageViewMenu_Total];//声明动作
	QString m_strMenuName[EImageViewMenu_Total];
	QMenu *m_menu;//声明菜单
	QMenu *m_menu_child;
	std::vector<std::function<void()>> m_fvec;

	for (auto v : m_action)
	{
		v = nullptr;
	}
	//配置菜单
	m_strMenuName[EImageViewMenu_Open] = "打开";
	m_strMenuName[EImageViewMenu_Save] = "保存";
	m_strMenuName[EImageViewMenu_Close] = "关闭";
	m_strMenuName[EImageViewMenu_ZoomIn] = "放大";
	m_strMenuName[EImageViewMenu_ZoomOut] = "缩小";
	m_strMenuName[EImageViewMenu_ZoomFit] = "适应";
	m_strMenuName[EImageViewMenu_Measure] = "测量";
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
		//connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(m_fvec.at(i)()));//无效？？？？？？？
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
		qDebug() << "摄像头未连接";
	} 
	else
	{
		qDebug() << "摄像头已连接";
	}
}

void ImageView::showstatus(bool status)
{
	if (!cameraReady) return;
	if (status)
		qDebug() << "预览中";
	else
		qDebug()<<"预览中止";
}

void ImageView::BuildConnet()
{
	//信号槽的参数是自定义的，这时需要用qRegisterMetaType注册一下这种类型
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
