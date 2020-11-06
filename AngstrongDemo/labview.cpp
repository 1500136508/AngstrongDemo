#pragma execution_character_set("utf-8")
#include <QMenu>
#include <QFileDialog>
#include <string>
#include "labview.h"
LabView::LabView(QWidget * pParent, Qt::WindowFlags f)
	:QLabel(pParent,f)
{
}

LabView::LabView(const QString &text, QWidget *parent /* = nullptr */, Qt::WindowFlags f /* = Qt::WindowFlags() */)
	:QLabel(text,parent,f)
{

}
LabView::~LabView()
{
}

bool LabView::Open()
{
	bool bReturn = false;

	do
	{
		try
		{
			QString filename;
			filename = QFileDialog::getOpenFileName(this,tr("选择图像"),"",tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
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
				//image=imread(fileName.toLatin1().data);
				cv::cvtColor(m_Image, m_Image, CV_BGR2RGB);
				cv::resize(m_Image, m_Image, cv::Size(300, 200));
				QImage img = QImage((const unsigned char*)(m_Image.data), m_Image.cols, m_Image.rows, QImage::Format_RGB888);

				setPixmap(QPixmap::fromImage(img));
				resize(QSize(img.width(), img.height()));
				setScaledContents(true);
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

	bool LabView::Save()
{
	return false;
}

bool LabView::Close()
{
	return false;
}

void LabView::contextMenuEvent(QContextMenuEvent * ev)
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

void LabView::on_Open_triggle()
{
	Open();
}

void LabView::on_Save_triggle()
{
	Save();
}

void LabView::on_Close_triggle()
{
	Close();
}
