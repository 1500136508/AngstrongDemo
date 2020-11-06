#pragma execution_character_set("utf-8")
#include <QMenu>
#include <QFileDialog>
#include "View.h"

View::View(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
{
	m_scene = new QGraphicsScene();
	m_imageItem = new QGraphicsPixmapItem();
	//场景增加画布
	m_scene->addItem(m_imageItem);
	//控件绑定场景
	setScene(m_scene);
	//对QGraphcisView控件注册事件响应
	installEventFilter(this);
	//使能QGraphcisView控件的鼠标跟踪
	setMouseTracking(true);
}

View::View(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	:QGraphicsView(scene,parent)
{

}

View::~View()
{
	/*if (m_scene)
	{
		delete m_scene;
		m_scene = nullptr;
	}

	if (m_imageItem)
	{
		delete m_imageItem;
		m_imageItem = nullptr;
	}*/
}

void View::contextMenuEvent(QContextMenuEvent * ev)
{
	//Create Menu
	QMenu m_menu;
	QAction Open("打开");
	QAction Save("保存");
	QAction Close("关闭");

	//QMenu *m_child_menu = new QMenu();
	//QAction *OpenImage = new QAction("打开图片");

	m_menu.addAction(&Open);
	//Open->setMenu(m_child_menu);
	//m_child_menu->addAction(OpenImage);
	//m_menu->addMenu(m_child_menu);
	m_menu.addAction(&Save);
	m_menu.addSeparator();
	m_menu.addAction(&Close);

	//绑定菜单触发事件
	connect(&Open, SIGNAL(triggered(bool)), this, SLOT(on_Open_triggle()));
	connect(&Save, SIGNAL(triggered(bool)), this, SLOT(on_Save_triggle()));
	connect(&Close, SIGNAL(triggered(bool)), this, SLOT(on_Close_triggle()));


	m_menu.exec(QCursor::pos());//在当前鼠标位置显示
}

void View::on_Open_triggle()
{
	Open();
}
void View::on_Save_triggle()
{
}

void View::on_Close_triggle()
{
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
				
				/*cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
				cv::resize(m_Image, m_Image, cv::Size(300, 200));
				QImage img = QImage((const unsigned char*)(m_Image.data), m_Image.cols, m_Image.rows, QImage::Format_RGB888);*/
				if (m_Image.type() == CV_8UC3)//三通道RGB
				{
					if (!m_imageItem)//防呆处理
					{
						break;
					}
					cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
					m_imageItem->setPixmap(QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_RGB888)));
				}
				else if (m_Image.type() == CV_8UC4)//四通道RGBA
				{
					if (!m_imageItem)
					{
						break;
					}
					m_imageItem->setPixmap(QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_RGBA8888_Premultiplied)));
				}
				else if (m_Image.type() == CV_8UC1)//单通道Gray
				{
					if (!m_imageItem)
					{
						break;
					}
					m_imageItem->setPixmap(QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_Grayscale8)));
				}
				else
				{
					break;
				}
				//更新显示
				show();
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
	return false;
}
