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
			filename = QFileDialog::getOpenFileName(this,tr("ѡ��ͼ��"),"",tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
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
