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

	//��ʼ��
	m_spScene = nullptr;
	m_spItem = nullptr;
	m_spPix = nullptr;
	
	//����Ϊ�Զ�������Ҽ��˵�ģʽ
	ui->m_gView_ImageView->setContextMenuPolicy(Qt::CustomContextMenu);
	//���ź�������Ӧ�Ĳۺ���
	connect(ui->m_gView_ImageView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

	//����ImageView�Ի������۴���
	setWindowFlags(Qt::FramelessWindowHint);

	//����QGraphicsView������Ԫ��
	m_spScene = std::make_shared<ImageScene>(new ImageScene());//���ӳ���
	m_spItem  = std::make_shared<ImageItem>(new ImageItem());//����ͼԪ
	m_spPix   = std::make_shared<ImagePix>(new ImagePix());//����ͼ��ͼԪ
	//m_spScene->setSceneRect(QRectF(0, 0, ui->m_gView_ImageView->width()+100, ui->m_gView_ImageView->height()+100));
	//m_spScene->setSceneRect(0, 0, 0, 0);
	m_spScene->addItem(m_spPix.get());//�����ԪԪ�ذ󶨵�Scene
	ui->m_gView_ImageView->setScene(m_spScene.get());//��QGraphicsView���Scene;

	//������겶��
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
	m_spScene->setSceneRect(0, 0, viewRect.width(), viewRect.height()); //������ԭ��������ʾ���ڵ����Ͻ�
	QPixmap pix = QPixmap::fromImage(qImage);
	m_spPix->setPixmap(pix.scaled(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio));
	//���þ�����ʾλ��
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
				
				//������ʾλ��
				ZoomFit();
				//QRect viewRect = ui->m_gView_ImageView->geometry();
				//m_spScene->setSceneRect(1, 1, viewRect.width() - 2, viewRect.height() - 2); //������ԭ��������ʾ���ڵ����Ͻ�
				////m_spScene->addRect(1, 1, viewRect.width() - 4, viewRect.height() - 4, QPen(Qt::red)); //��ɫ���������������

				//m_spPix->setPos((viewRect.width() - m_spPix->pixmap().width()) / 2,
				//	(viewRect.height() - m_spPix->pixmap().height()) / 2); //�趨ͼƬ�ڳ����е�����
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
