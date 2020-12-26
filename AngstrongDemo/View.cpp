#pragma execution_character_set("utf-8")
#include "View.h"
#include <QMenu>
#include <QFileDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTextStream>
#include <QPushButton>
#include <QDebug>

View::View(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
{
	//初始化
	m_spScene = nullptr;
	m_spItem = nullptr;
	m_spPix = nullptr;
	m_ImageWidth = 0;
	m_ImageHeight = 0;
	m_vecAvgROI.clear();
	first_time_to_live_ = true;

	//增加QGraphicsView框架相关元素
	m_spScene = std::make_shared<ImageScene>(new ImageScene());//增加场景
	m_spItem = std::make_shared<ImageItem>(new ImageItem());//增肌图元
	m_spPix = std::make_shared<ImagePix>(new ImagePix());//增加图像图元
	

	m_spScene->addItem(m_spPix.get());//添加像元元素绑定到Scene
	setScene(m_spScene.get());//将QGraphicsView添加Scene;

	//qss 皮肤美化
	QFile file("black.qss");
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
	m_vecAvgROI.clear();
}

int View::GetImageWidth() const
{
	return m_ImageWidth;
}

int View::GetImageHeight() const
{
	return m_ImageHeight;
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

	if (m_spPix->pixmap().isNull())
	{
		return;
	}
	QRectF viewRect = geometry();
	m_spScene->setSceneRect(0, 0, viewRect.width(), viewRect.height()); //将坐标原点设在显示窗口的左上角
	QPixmap pix = QPixmap::fromImage(qImage);
	QPixmap pix_sacle = pix.scaled(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio);
	float w = pix_sacle.width()*1.0 / m_ImageWidth * 1.0;
	float h = pix_sacle.height()*1.0 / m_ImageHeight * 1.0;
	m_spPix->SetFit(w);
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

std::shared_ptr<ImageScene> View::GetScene() const
{
	return m_spScene;
}

void View::ClearAll()
{
	QRect view_rect = geometry();
	QList<QGraphicsItem *> item_list_p = m_spScene->items(QRectF(0, 0, view_rect.width(), view_rect.height()), Qt::IntersectsItemShape);

	//删除元素
	for (int i = 0; i < item_list_p.size(); i++)
	{
		m_spScene->removeItem(item_list_p[i]);  //从scene移除		
		//if (item_list_p[i])
		//{
		//	delete item_list_p[i];  //释放内存
		//	item_list_p[i] = nullptr;
		//}
	}

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
#ifdef DEBUG
				qDebug() << "ImageWidth:" << m_ImageWidth << "ImageHeight:" << m_ImageHeight;
#endif

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
	if (m_spPix)
	{
		if (!m_spPix->pixmap().isNull())
		{
			CalcInfo(event->localPos());
			CalcAvgArea();
		}
	}
	
	m_spScene->update();
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
	bool add_to_main_mune = true;
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
	m_fvec.push_back(std::bind(&View::on_open_clicked, this));
	m_fvec.push_back(std::bind(&View::on_save_clicked, this));
	m_fvec.push_back(std::bind(&View::on_close_clicked, this));
	m_fvec.push_back(std::bind(&View::on_zoomIn_clicked, this));
	m_fvec.push_back(std::bind(&View::on_zoomOut_clicked, this));
	m_fvec.push_back(std::bind(&View::on_zoomFit_clicked, this));
	m_fvec.push_back(std::bind(&View::on_measure_clicked, this));
	m_fvec.push_back(std::bind(&View::on_measureRect_clicked, this));
	m_fvec.push_back(std::bind(&View::on_measureCircle_clicked, this));
	m_menu = new QMenu();
	m_menu_child_measure = new QMenu();
	for (size_t i = EImageViewMenu_Open; i < EImageViewMenu_Total; ++i)
	{
		m_action[i] = new QAction(m_strMenuName[i]);
		switch (i)
		{
		case EImageViewMenu_ZoomIn:
		{
			m_menu->addSeparator();
		}
			break;
		case EImageViewMenu_Child_MeasureRect:
		case EImageViewMenu_Child_MeasureCircle:
		{
			m_menu_child_measure->addAction(m_action[i]);
			add_to_main_mune = false;
		}
			break;
		default:
			break;
		}
		if (add_to_main_mune)
		{
			m_menu->addAction(m_action[i]);
		}
		connect(m_action[i], &QAction::triggered, this, m_fvec.at(i));
		add_to_main_mune = true;
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

		m_spRect->setROIRect(QRect(0, 0, 0, 0));
	}
}

void View::on_zoomIn_clicked()
{
	setCursor(Qt::SizeAllCursor);
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
		m_spRect->setTransformOriginPoint(25, 25);
		m_spRect->setFocus(Qt::MouseFocusReason);
		m_spScene->addItem(m_spRect.get());
		m_spRect->setROIRect(QRect(50, 50, 100, 100));
	}
	else
	{
		if (m_spRect->rect().width() == 0 && m_spRect->rect().height() == 0)
		{
			m_spRect->setROIRect(QRect(50, 50, 100, 100));
		}
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
			m_spPix->setScale(m_spPix->GetScale());
			if (first_time_to_live_)
			{
				ZoomFit();
				first_time_to_live_ = false;
			}
			emit SendImageInfo(true, m_ImageWidth, m_ImageHeight);
		}
	}
}

void View::ReceiveCreateAvgArea(int nIndex, bool bIsCreate)
{
	if (bIsCreate)
	{
		if (m_vecAvgROI.size() == 0)//在第一次创建时，直接往容器里放入两个ROI对象,至程序结束自动销毁
		{
			m_vecAvgROI.push_back(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()));
			m_vecAvgROI.push_back(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()));

			for (size_t sz = 0; sz < m_vecAvgROI.size(); sz++)
			{
				m_vecAvgROI[sz]->setFocus(Qt::MouseFocusReason);
				m_vecAvgROI[sz]->setTitle("ROI_0" + QString::number(sz));
				m_spScene->addItem(m_vecAvgROI[sz].get());
			}
		}

		if (m_vecAvgROI[nIndex - 1])
		{
			if (m_spPix)
			{
				if (!m_spPix->pixmap().isNull())
				{
					m_vecAvgROI[nIndex - 1]->setROIRect(QRect(0, 0, 50, 50));
					CalcAvgArea();
				}
			}
			
		}
	}
	else
	{
		if (m_vecAvgROI.size() <= 0)
		{
			return;
		}
		if (m_vecAvgROI[nIndex - 1])
		{
			m_vecAvgROI[nIndex - 1]->setROIRect(QRect(0, 0, 0, 0));
			m_spScene->update();
			emit SendAvgArea(nIndex, m_vecAvgROI[nIndex - 1]->rect());
		}
	}
}

void View::CalcInfo(QPointF point)
{
	qreal view_x = point.x();
	qreal view_y = point.y();

	QPointF qPoint1 = m_spPix->scenePos();//图像坐标
	QPointF qPoint2 = mapToScene(view_x, view_y);//当前鼠标坐标
	qreal imageW = m_spPix->pixmap().width()*m_spPix->GetScale();
	qreal imageH = m_spPix->pixmap().height()*m_spPix->GetScale();
	qreal ratio_x = m_ImageWidth / (imageW*1.0);
	qreal ratio_y = m_ImageHeight / (imageH*1.0);
	if (qPoint2.x() >= qPoint1.x() && qPoint2.x() <= qPoint1.x() + imageW &&
		qPoint2.y() >= qPoint1.y() && qPoint2.y() <= qPoint1.y() + imageH)//判断鼠标是否在图像上
	{

		QPointF p1((qPoint2.x() - qPoint1.x()), (qPoint2.y() - qPoint1.y()));
		qreal x = p1.x() * ratio_x;
		qreal y = p1.y() * ratio_y;

		if (qImage.valid(x, y))//判断坐标是否有效
		{
			QColor color = qImage.pixel(floor(x), floor(y));
			int GrayValue_R = color.red();
			int GrayValue_G = color.green();
			int GrayValue_B = color.blue();
			emit SendImageGray(GrayValue_R, GrayValue_G, GrayValue_B);
#ifdef DEBUG
			qDebug() << "X:" << x << "Y:" << y << "R:" << GrayValue_R << " G:" << GrayValue_G << " B:" << GrayValue_B;
#endif
		}
		emit SendMouseInfo(floor(x), floor(y));
	}
	else
	{
#ifdef DEBUG
		qDebug() << "out of range";
#endif
		emit SendMouseInfo(-1, -1);
	}
}

void View::CalcAvgArea()
{
	if (m_vecAvgROI.size() > 0)
	{
		QPointF point_rect;
		QRectF rect_pix;
		QRectF rect_roi;
		bool bCancel[2] = { false };
		//以下坐标全部相对于Scene
		QPointF point_pix = m_spPix->scenePos();//获取图像左上角坐标（以Scene左上角为原点)
		//计算出整个图像区域的rect（此时的图像是自适应窗口的，可能被拉伸或者压缩了，下边需要进行转换）
		QRect view_rect = geometry();
		float ratio_x = (m_ImageWidth*1.0) / view_rect.width();
		float ratio_y = (m_ImageHeight *1.0) / view_rect.height();

		for (int nIndex = 0; nIndex < 2; nIndex++)
		{
			point_rect = m_vecAvgROI[nIndex]->rect().center();//获取roi中心坐标（相对于Scene)
			//横、纵向压缩对比情况处理
			if (ratio_x > ratio_y)
			{
				float h = (m_ImageHeight*1.0) * m_spPix->GetScale();//根据压缩比，计算出图像高度
				rect_pix = QRectF(point_pix, QPointF(view_rect.width(), (h + point_pix.y())));
				
			}
			else
			{
				float w = (m_ImageWidth*1.0)*m_spPix->GetScale();
				rect_pix = QRectF(point_pix, QPointF(w + point_pix.x(), view_rect.height()));
			}
			if (rect_pix.contains(m_vecAvgROI[nIndex]->rect()))
			{
				qreal top_left_x = (m_vecAvgROI[nIndex]->rect().topLeft().x() - point_pix.x())/m_spPix->GetScale();
				qreal top_left_y = (m_vecAvgROI[nIndex]->rect().topLeft().y() - point_pix.y())/m_spPix->GetScale();
				rect_roi.setTopLeft(QPointF(top_left_x, top_left_y));
				rect_roi.setWidth(m_vecAvgROI[nIndex]->rect().width()/m_spPix->GetScale());
				rect_roi.setHeight(m_vecAvgROI[nIndex]->rect().height()/m_spPix->GetScale());

				emit SendAvgArea(nIndex, rect_roi);
			}
			else
			{
				bCancel[nIndex] = true;
			}
			if (bCancel[0] && bCancel[1])
			{
				emit SendAvgArea(-1, QRectF(0, 0, 0, 0));
			}
		}
	}
}

void View::ReceiveIsTheFirstTimeToLive(bool is_first_time_to_live)
{
	first_time_to_live_ = is_first_time_to_live;
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
