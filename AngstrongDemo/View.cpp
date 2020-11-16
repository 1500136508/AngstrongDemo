#pragma execution_character_set("utf-8")
#include <QMenu>
#include <QFileDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTextStream>
#include <QDebug>
#include "View.h"

View::View(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
{
	//初始化
	m_spScene = nullptr;
	m_spItem = nullptr;
	m_spPix = nullptr;
	m_ImageWidth = 0;
	m_ImageHeight = 0;

	//增加QGraphicsView框架相关元素
	m_spScene = std::make_shared<ImageScene>(new ImageScene());//增加场景
	m_spItem = std::make_shared<ImageItem>(new ImageItem());//增肌图元
	m_spPix = std::make_shared<ImagePix>(new ImagePix());//增加图像图元
	

	m_spScene->addItem(m_spPix.get());//添加像元元素绑定到Scene
	setScene(m_spScene.get());//将QGraphicsView添加Scene;

	//qss 皮肤美化
	QFile file("../qss/black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	setStyleSheet(stylesheet);

	setMouseTracking(true);
}

View::View(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	:QGraphicsView(scene,parent)
{

}

View::~View()
{
}

void View::Zoom(QPointF pointF, double fScale)
{
	/*if (m_spPix)
	{
		m_spPix->setScale(fScale);
	}*/
}

void View::ZoomIn(QPointF poinF, double fScale)
{
	
}

void View::ZoomOut(QPointF pointF, double fScale)
{
}

void View::ZoomFit()
{
	if (!m_spScene || !m_spPix)
	{
		return;
	}

	QRect viewRect = geometry();
	m_spScene->setSceneRect(0, 0, viewRect.width(), viewRect.height()); //将坐标原点设在显示窗口的左上角
	QPixmap pix = QPixmap::fromImage(qImage);
	QPixmap pix_sacle = pix.scaled(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio);
	m_spPix->setPixmap(pix_sacle);
	m_spPix->SetFit();
	//设置居中显示位置
	int pWidth = pix_sacle.width();
	int pHeight = pix_sacle.height();
	int vWidth = viewRect.width();
	int vHeight = viewRect.height();
	if (vWidth > pWidth)
	{
		qreal fDeta_x = (vWidth - pWidth) / 2;
		m_spPix->setPos(fDeta_x, 0);
	}
	else if (vHeight > pHeight)
	{
		qreal fDeta_y = (vHeight - pHeight) / 2;
		m_spPix->setPos(0, fDeta_y);
	}
}

float View::getLength2(const QPointF & point1, const QPointF & point2)
{
	return (point1.x() - point2.x()) * (point1.x() - point2.x()) + (point1.y() - point2.y()) * (point1.y() - point2.y());
}

bool View::Open()
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
				m_ImageWidth = qImage.width();
				m_ImageHeight = qImage.height();
				m_spPix->setPixmap(QPixmap::fromImage(qImage));
				qDebug() << "ImageWidth:" << m_ImageWidth << "ImageHeight:"<<m_ImageHeight;

				//处理显示位置,默认为图像居中显示
				ZoomFit();
				emit SendImageInfo(true, m_ImageWidth, m_ImageHeight);//发送图像信息
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
	bool bReturn = false;
	
	do 
	{
		if (!qImage.isNull())
		{
			QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.gif *.tiff")); //选择路径
			qImage.save(filename);
		}
		else
		{
			break;
		}

		bReturn = true;
	} while (false);

	return bReturn;
}

void View::mouseMoveEvent(QMouseEvent * event)
{
	if (m_spRect)
	{
		if (m_bTurn != true)
		{
			QCursor cursor = m_spRect->getRotateCursor(mapToScene(event->pos()));
			if (!cursor.pixmap().isNull())
			{
				viewport()->setCursor(cursor);
			}
			else if (!m_spRect->isHover())
			{
				viewport()->setCursor(Qt::ArrowCursor);
			}
		}
		else
		{
			m_spRect->setRotateEnd(mapToScene(event->pos()));
		}
	}

	//判断鼠标是否在ImageItem上
	if (m_spPix)
	{
		if (!m_spPix->pixmap().isNull())//图像不为空
		{
			int x = event->pos().x();
			int y = event->pos().y();

			QPointF qPoint1 = m_spPix->scenePos();
			QPointF qPoint2 = mapToScene(x, y);
			int imageW = m_spPix->pixmap().width();
			int imageH = m_spPix->pixmap().height();
			qreal ratio_x = m_ImageWidth / (imageW*1.0);
			qreal ratio_y = m_ImageHeight / (imageH*1.0);
			if (qPoint2.x() >= qPoint1.x() && qPoint2.x() <= qPoint1.x() + imageW &&
				qPoint2.y() >= qPoint1.y() && qPoint2.y() <= qPoint1.y() + imageH)//判断鼠标是否在图像上
			{
				int x = (qPoint2.x() - qPoint1.x())*ratio_x;//将缩放后的图像坐标转换为实际的图像坐标
				int y = qPoint2.y() * ratio_y;
				if (qImage.valid(x,y))//判断坐标是否有效
				{
					QColor color = qImage.pixel(x, y);
					int GrayValue_R = color.red();
					int GrayValue_G = color.green();
					int GrayValue_B = color.blue();
					qDebug() << "X:" << x << "Y:" << y << "R:" << GrayValue_R << " G:" << GrayValue_G << " B:" << GrayValue_B;
					emit SendMouseInfo(x, y);
					emit SendImageGray(GrayValue_R, GrayValue_G, GrayValue_B);
				}
			}
			else
			{
				qDebug() << "out of range";
				emit SendMouseInfo(-1, -1);
			}
		}
	}

	QGraphicsView::mouseMoveEvent(event);
}

void View::mousePressEvent(QMouseEvent * event)
{
	if (m_spRect)
	{
		if (!m_spRect->getRotateCursor(mapToScene(event->pos())).pixmap().isNull())
		{
			m_spRect->setRotateStart(mapToScene(event->pos()));
			m_bTurn = true;
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent * event)
{
	if (true == m_bTurn)
	{
		m_bTurn = false;
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void View::wheelEvent(QWheelEvent * event)
{
	return QGraphicsView::wheelEvent(event);
}

void View::paintEvent(QPaintEvent * event)
{
	return QGraphicsView::paintEvent(event);
}

void View::contextMenuEvent(QContextMenuEvent * event)
{
	QAction *m_action[EImageViewMenu_Total];//声明动作
	QString m_strMenuName[EImageViewMenu_Total];
	QMenu *m_menu;//声明菜单
	QMenu *m_menu_child_measure;//测量子菜单
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
	m_strMenuName[EImageViewMenu_Child_MeasureRect] = "矩形";
	m_strMenuName[EImageViewMenu_Child_MeasureCircle] = "圆形";
	/*m_fvec.push_back(std::bind(&ImageView::on_open_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_save_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_close_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_zoomIn_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_zoomOut_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_zoomFit_clicked, this));
	m_fvec.push_back(std::bind(&ImageView::on_measure_clicked, this));*/
	m_menu = new QMenu();
	m_menu_child_measure = new QMenu();
	for (size_t i = EImageViewMenu_Open; i < EImageViewMenu_Total; ++i)
	{
		m_action[i] = new QAction(m_strMenuName[i]);
		//connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(m_fvec.at(i)()));//无效？？？？？？？

		switch (i)
		{
		case EImageViewMenu_Open:
		{
			m_menu->addAction(m_action[i]);//主菜单
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_open_clicked()));
		}
		break;
		case EImageViewMenu_Save:
		{
			m_menu->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_save_clicked()));
		}
		break;
		case EImageViewMenu_Close:
		{
			m_menu->addAction(m_action[i]);
			m_menu->addSeparator();
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_close_clicked()));
		}
		break;
		case EImageViewMenu_ZoomIn:
		{
			m_menu->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_zoomIn_clicked()));
		}
		break;
		case EImageViewMenu_ZoomOut:
		{
			m_menu->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_zoomOut_clicked()));
		}
		break;
		case EImageViewMenu_ZoomFit:
		{
			m_menu->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_zoomFit_clicked()));
		}
		break;
		case EImageViewMenu_Measure:
		{
			m_menu->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_measure_clicked()));
		}
		break;
		case EImageViewMenu_Child_MeasureRect:
		{
			m_menu_child_measure->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_measureRect_clicked()));
		}
		break;
		case EImageViewMenu_Child_MeasureCircle:
		{
			m_menu_child_measure->addAction(m_action[i]);
			connect(m_action[i], SIGNAL(triggered(bool)), this, SLOT(on_measureCircle_clicked()));
		}
		break;
		default:
			break;
		}
	}
	m_action[EImageViewMenu_Measure]->setMenu(m_menu_child_measure);//父菜单添加子菜单
	m_menu->addMenu(m_menu_child_measure);//将子菜单添加到主菜单
	m_menu->addMenu(m_menu_child_measure);//将子菜单添加到主菜单
	m_menu->setStyleSheet(stylesheet);
	m_menu_child_measure->setStyleSheet("background-color: rgba(74, 79, 81, 1); color: rgb(187, 187, 187);selection-background-color:#4b6eaf; ");
	m_menu->exec(QCursor::pos());//显示菜单

	//内存清理与释放
	if (m_menu)
	{
		delete m_menu;
		m_menu = nullptr;
	}
	if (m_menu_child_measure)
	{
		delete m_menu_child_measure;
		m_menu_child_measure = nullptr;
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

void View::on_open_clicked()
{
	Open();
}

void View::on_save_clicked()
{
	Save();
}

void View::on_close_clicked()
{
	if (m_spRect)
	{
		QList<QGraphicsItem*> m_list_item = m_spScene->selectedItems();
		for (auto i = 0; i < m_list_item.size(); ++i)
		{
			m_spScene->removeItem(m_list_item[i]);
			m_spRect = nullptr;
		}
	}
}

void View::on_zoomIn_clicked()
{
}

void View::on_zoomOut_clicked()
{
}

void View::on_zoomFit_clicked()
{
	ZoomFit();
}

void View::on_measure_clicked()
{
}

void View::on_measureRect_clicked()
{
	//设置矩形图元特性
	if (!m_spRect)
	{
		m_spRect = std::make_shared<GraphicsRectItem>(new GraphicsRectItem());
		m_spRect->setPos(width() / 2 - 50, height() / 2 - 50);
		m_spRect->setRect(0, 0, 100, 100);
		m_spRect->setTransformOriginPoint(25, 25);
		//m_spRect->setRotation(45);
		m_spRect->setFocus(Qt::MouseFocusReason);
		m_spScene->addItem(m_spRect.get());
	}
}

void View::on_measureCircle_clicked()
{
}

void View::SetImage(cv::Mat mat)
{
	if (m_spPix)
	{
		qImage = cvMat2QImage(mat);
		m_ImageWidth = qImage.width();
		m_ImageHeight = qImage.height();
		if (!qImage.isNull())
		{
			m_spPix->setPixmap(QPixmap::fromImage(qImage));

			ZoomFit();
			emit SendImageInfo(true, m_ImageWidth, m_ImageHeight);
		}
	}
}

QImage View::cvMat2QImage(const cv::Mat & mat)
{
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
