#pragma execution_character_set("utf-8")

#include <windows.h>
#include "AngstrongDemo.h"

AngstrongDemo::AngstrongDemo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	BuildConnect();

	setCentralWidget(&m_ImageView);
	//setCentralWidget(&m_CameraView);
	CreateDockWindow();
	AddToolBar();

	//�źŲ۵Ĳ������Զ���ģ���ʱ��Ҫ��qRegisterMetaTypeע��һ����������
	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::string>("std::string");
}

AngstrongDemo::~AngstrongDemo()
{
}

void AngstrongDemo::CreateDockWindow()
{
	//����camerlistͣ������
	QDockWidget *m_dock_cameralist = new QDockWidget(tr("CameraList"));
	m_dock_cameralist->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_cameralist->setAllowedAreas(Qt::LeftDockWidgetArea/* | Qt::RightDockWidgetArea*/);
	m_dock_cameralist->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::LeftDockWidgetArea, m_dock_cameralist);
	m_dock_cameralist->setWidget(&m_CameraView);
	//����parameterͣ������
	QDockWidget *m_dock_paramlist = new QDockWidget(tr("ParameterList"));
	m_dock_paramlist->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_paramlist->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::RightDockWidgetArea);
	m_dock_paramlist->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::RightDockWidgetArea, m_dock_paramlist);
	m_dock_paramlist->setWidget(&m_ParamView);
	//����Outputͣ������
	QDockWidget *m_dock_output = new QDockWidget(tr("Output"));
	m_dock_output->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_output->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::BottomDockWidgetArea);
	m_dock_output->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::BottomDockWidgetArea, m_dock_output);
	m_dock_output->setWidget(&m_OutputView);
	//����ImageViewͣ������
	//QDockWidget *m_dock_imageview = new QDockWidget(tr("ImageView"));
	//m_dock_imageview->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	////m_dock_imageview->setAllowedAreas(Qt::LeftDockWidgetArea/* | Qt::BottomDockWidgetArea*/);
	//m_dock_imageview->setAllowedAreas(Qt::AllDockWidgetAreas);
	//addDockWidget(Qt::TopDockWidgetArea, m_dock_imageview);
	//m_dock_imageview->setWidget(&m_ImageView);
}

void AngstrongDemo::AddToolBar()
{
	QToolBar* pToolBar = ui.mainToolBar;

	QAction* pActionC = new QAction(QIcon(QPixmap(":/AngstrongDemo/image_ico/Option_normal.png")), "��");

	pToolBar->addAction(pActionC);

	//QObject::connect(pActionC, SIGNAL(triggered(bool)), this, SLOT(onMsg()));
}

void AngstrongDemo::BuildConnect()
{
	//connect(&m_ImageView.ui->m_gView_ImageView->m_reader, SIGNAL(sendImage(cv::Mat)), m_ImageView.ui->m_gView_ImageView, SLOT(setImage(cv::Mat)));
}

void AngstrongDemo::run_camera()
{
}
