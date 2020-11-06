#pragma execution_character_set("utf-8")
#include <QMenu>
#include <QFileDialog>
#include "View.h"

View::View(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
{
	m_scene = new QGraphicsScene();
	m_imageItem = new QGraphicsPixmapItem();
	//�������ӻ���
	m_scene->addItem(m_imageItem);
	//�ؼ��󶨳���
	setScene(m_scene);
	//��QGraphcisView�ؼ�ע���¼���Ӧ
	installEventFilter(this);
	//ʹ��QGraphcisView�ؼ���������
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
	QAction Open("��");
	QAction Save("����");
	QAction Close("�ر�");

	//QMenu *m_child_menu = new QMenu();
	//QAction *OpenImage = new QAction("��ͼƬ");

	m_menu.addAction(&Open);
	//Open->setMenu(m_child_menu);
	//m_child_menu->addAction(OpenImage);
	//m_menu->addMenu(m_child_menu);
	m_menu.addAction(&Save);
	m_menu.addSeparator();
	m_menu.addAction(&Close);

	//�󶨲˵������¼�
	connect(&Open, SIGNAL(triggered(bool)), this, SLOT(on_Open_triggle()));
	connect(&Save, SIGNAL(triggered(bool)), this, SLOT(on_Save_triggle()));
	connect(&Close, SIGNAL(triggered(bool)), this, SLOT(on_Close_triggle()));


	m_menu.exec(QCursor::pos());//�ڵ�ǰ���λ����ʾ
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
			filename = QFileDialog::getOpenFileName(this, tr("ѡ��ͼ��"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
			if (filename.isEmpty())
			{
				break;
			}
			else
			{
				//QImage* img = new QImage;

				//if (!(img->load(filename))) //����ͼ��
				//{
				//	QMessageBox::information(this,
				//		tr("��ͼ��ʧ��"),
				//		tr("��ͼ��ʧ��!"));
				//	delete img;
				//	return;
				//}

				std::string str = filename.toStdString();  // ��filenameת��Ϊstring���ͣ�
				m_Image = cv::imread(str);
				
				/*cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
				cv::resize(m_Image, m_Image, cv::Size(300, 200));
				QImage img = QImage((const unsigned char*)(m_Image.data), m_Image.cols, m_Image.rows, QImage::Format_RGB888);*/
				if (m_Image.type() == CV_8UC3)//��ͨ��RGB
				{
					if (!m_imageItem)//��������
					{
						break;
					}
					cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
					m_imageItem->setPixmap(QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_RGB888)));
				}
				else if (m_Image.type() == CV_8UC4)//��ͨ��RGBA
				{
					if (!m_imageItem)
					{
						break;
					}
					m_imageItem->setPixmap(QPixmap::fromImage(QImage((const unsigned char*)m_Image.data, m_Image.cols, m_Image.rows, QImage::Format::Format_RGBA8888_Premultiplied)));
				}
				else if (m_Image.type() == CV_8UC1)//��ͨ��Gray
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
				//������ʾ
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
