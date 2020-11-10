#pragma execution_character_set("utf-8")
#include <QDebug>
#include <QPoint>
#include <QFileDialog>
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
	
	//设置为自定义鼠标右键菜单模式
	ui->m_gView_ImageView->setContextMenuPolicy(Qt::CustomContextMenu);
	//给信号设置相应的槽函数
	connect(ui->m_gView_ImageView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

	//进行ImageView对话框的外观处理
	setWindowFlags(Qt::FramelessWindowHint);

	//增加QGraphicsView框架相关元素
	m_spScene = std::make_shared<ImageScene>(new ImageScene());//增加场景
	m_spItem  = std::make_shared<ImageItem>(new ImageItem());//增肌图元
	m_spPix   = std::make_shared<ImagePix>(new ImagePix());//增加图像图元
	//m_spScene->setSceneRect(QRectF(0, 0, ui->m_gView_ImageView->width()+100, ui->m_gView_ImageView->height()+100));
	//m_spScene->setSceneRect(0, 0, 0, 0);
	m_spScene->addItem(m_spPix.get());//添加像元元素绑定到Scene
	ui->m_gView_ImageView->setScene(m_spScene.get());//将QGraphicsView添加Scene;

	//启动鼠标捕获
	setMouseTracking(true);
}

ImageView::~ImageView()
{
    delete ui;
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
	double fRatio_w = pWidth / vWidth;
	double fRatio_h = pHeight / vHeight;
	if (fRatio_w <= fRatio_h)
	{
		int nDeta_x = (viewRect.width() - m_spPix->pixmap().width())/2;
		m_spPix->setPos(nDeta_x, 0);
	}
	else if(fRatio_w < fRatio_h)
	{
		int nDeta_y = (viewRect.height() - m_spPix->pixmap().height())/2;
		m_spPix->setPos(0, nDeta_y);
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
				
				//处理显示位置
				ZoomFit();
				//QRect viewRect = ui->m_gView_ImageView->geometry();
				//m_spScene->setSceneRect(1, 1, viewRect.width() - 2, viewRect.height() - 2); //将坐标原点设在显示窗口的左上角
				////m_spScene->addRect(1, 1, viewRect.width() - 4, viewRect.height() - 4, QPen(Qt::red)); //红色方框标明场景区域

				//m_spPix->setPos((viewRect.width() - m_spPix->pixmap().width()) / 2,
				//	(viewRect.height() - m_spPix->pixmap().height()) / 2); //设定图片在场景中的坐标
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
	int a = 7;
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

QImage ImageView::cvMat2QImage(const cv::Mat & mat)
{
	if (mat.type() == CV_8UC1)
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
