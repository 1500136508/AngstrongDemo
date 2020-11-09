#pragma execution_character_set("utf-8")
#include <QMenu>
#include <QFileDialog>
#include "View.h"

View::View(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
{
	m_scene = new ImageScene();
	m_ImageItem = new ImagItem();
	//场景增加画布
	m_scene->addItem(m_ImageItem);
	//控件绑定场景
	setScene(m_scene);
	//对QGraphcisView控件注册事件响应
	installEventFilter(this);
	////使能QGraphcisView控件的鼠标跟踪
	setMouseTracking(true);
}

View::View(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	:QGraphicsView(scene,parent)
{

}

View::~View()
{
	if (m_scene)
	{
		delete m_scene;
		m_scene = nullptr;
	}

	/*if (m_ImageItem)
	{
		delete m_ImageItem;
		m_ImageItem = nullptr;
	}*/
}

void View::contextMenuEvent(QContextMenuEvent * ev)
{
	//Create Menu
	QMenu m_menu;
	QAction Open("打开");
	QAction Save("保存");
	QAction Close("关闭");
	QAction ZoomIn("放大");
	QAction ZoomOut("缩小");
	QAction ZoomFit("适应");
	QAction Measure("测量");

	QMenu m_child_menu;
	QAction Line("直线");
	QAction Angle("角度");
	QAction Radian("弧度");

	//一级菜单
	m_menu.addAction(&Open);
	m_menu.addAction(&Save);
	m_menu.addAction(&Close);
	m_menu.addSeparator();
	m_menu.addAction(&ZoomIn);
	m_menu.addAction(&ZoomOut);
	m_menu.addAction(&ZoomFit);
	m_menu.addAction(&Measure);
	//二级菜单
	Measure.setMenu(&m_child_menu);
	m_child_menu.addAction(&Line);
	m_child_menu.addAction(&Angle);
	m_child_menu.addAction(&Radian);
	m_menu.addMenu(&m_child_menu);

	//绑定一级菜单触发事件
	connect(&Open, SIGNAL(triggered(bool)), this, SLOT(on_Open_triggle()));
	connect(&Save, SIGNAL(triggered(bool)), this, SLOT(on_Save_triggle()));
	connect(&Close, SIGNAL(triggered(bool)), this, SLOT(on_Close_triggle()));
	//绑定一级菜单触发事件
	//待续....


	m_menu.exec(QCursor::pos());//在当前鼠标位置显示
}

void View::mouseMoveEvent(QMouseEvent * event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void View::mousePressEvent(QMouseEvent * event)
{
	QGraphicsView::mousePressEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent * event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void View::on_Open_triggle()
{
	Open();
}
void View::on_Save_triggle()
{
	Save();
}

void View::on_Close_triggle()
{
	Close();
}

bool View::Open()
{
	bool bReturn = false;

	do 
	{
		try
		{
			QString filename;
			filename = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
			if (filename.isEmpty())
			{
				break;
			}
			else
			{
				//QImage* img = new QImage;

				//if (!(img->load(filename))) //加载图像
				//{
				//	QMessageBox::information(this,
				//		tr("打开图像失败"),
				//		tr("打开图像失败!"));
				//	delete img;
				//	return;
				//}

				std::string str = filename.toStdString();  // 将filename转变为string类型；
				m_Image = cv::imread(str);
				
				QImage qImage = cvMat2QImage(m_Image);
				m_ImageItem->SetImage(qImage);
				int nwidth = this->width();
				int nheight = this->height();
				m_ImageItem->setQGraphicsViewWH(nwidth, nheight);
				/*cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
				cv::resize(m_Image, m_Image, cv::Size(300, 200));
				QImage img = QImage((const unsigned char*)(m_Image.data), m_Image.cols, m_Image.rows, QImage::Format_RGB888);*/
				//if (m_Image.type() == CV_8UC3)//三通道RGB
				//{
				//	if (!m_ImageItem)//防呆处理
				//	{
				//		break;
				//	}
				//	cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
				//	m_ImageItem->SetImage(&QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_RGB888)));
				//}
				//else if (m_Image.type() == CV_8UC4)//四通道RGBA
				//{
				//	if (!m_ImageItem)
				//	{
				//		break;
				//	}
				//	m_ImageItem->SetImage(&QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_RGBA8888_Premultiplied)));
				//}
				//else if (m_Image.type() == CV_8UC1)//单通道Gray
				//{
				//	if (!m_ImageItem)
				//	{
				//		break;
				//	}
				//	m_ImageItem->SetImage(&QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_Grayscale8)));
				//}
				//else
				//{
				//	break;
				//}
				////更新显示
				//show();
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

bool View::Save()
{
	return false;
}

bool View::Close()
{
	m_ImageItem->ResetItemPos();
	m_ImageItem->setQGraphicsViewWH(m_ImageItem->GetImageWidth(), m_ImageItem->GetImageHeight());
	return false;
}

QImage View::cvMat2QImage(const cv::Mat & mat)
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

cv::Mat View::QImage2cvMat(QImage image)
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
