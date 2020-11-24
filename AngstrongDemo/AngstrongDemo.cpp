#pragma execution_character_set("utf-8")

#include <windows.h>
#include <QDebug>
#include <dbt.h>
#include <QTimer>
#include <QPalette>
#include <QString>
#include <QDesktopWidget>
#include "AngstrongDemo.h"

AngstrongDemo::AngstrongDemo(QWidget *parent)
	: QMainWindow(parent)
{
	registerDevice();//ע��USB����¼�
	ui.setupUi(this);

	//��ʼ��
	m_mpImageView.clear();
	m_pMainImageView = nullptr;
	m_pMainImageView = new ImageView();

	//�źŲ۵Ĳ������Զ���ģ���ʱ��Ҫ��qRegisterMetaTypeע��һ����������
	qRegisterMetaType<ECameraStatus>("ECameraStatus");
	BuildConnect();

	setCentralWidget(m_pMainImageView);
	CreateDockWindow();
	AddToolBar();

	//������۴���
	//QWidget* p = takeCentralWidget();
	//if (p)
	//{
	//	delete p;//ȥ��������м䴰�ڣ�ʵ����������ͣ��
	//}
	//setWindowFlags(Qt::FramelessWindowHint);//ȥ���߿�
	setDockNestingEnabled(true);//��DockǶ�׹���

	//Ƕ��qss��������
	QFile file("black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	setStyleSheet(stylesheet);
	m_pMainImageView->setStyleSheet(stylesheet);

	//��ʼ��ʶ������б�
	InitCamera();

	//������ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
}

AngstrongDemo::~AngstrongDemo()
{
	if (!m_mpImageView.empty())
	{
		for (auto mp : m_mpImageView)
		{
			if (mp.second.second)
			{
				mp.second.second->close();
				delete mp.second.second;
				mp.second.second = nullptr;
			}
		}
		m_mpImageView.clear();
	}
	if (m_pMainImageView)
	{
		delete m_pMainImageView;
		m_pMainImageView = nullptr;
	}
}

bool AngstrongDemo::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG* msg = reinterpret_cast<MSG*>(message);
	int msgType = msg->message;
	if (msgType == WM_DEVICECHANGE)
	{
		InitCamera();
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
		switch (msg->wParam)
		{
		case DBT_DEVICEARRIVAL:
		{
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv->dbcv_flags == 0)
				{
					QString USBDisk = QString(this->FirstDriveFromMask(lpdbv->dbcv_unitmask));
					qDebug() << "USB_Arrived and The USBDisk is: " << USBDisk;
					USBDevice usbDevice;
					//��ʱ��U����Ϣ�洢��m_usbList
					if (m_usbList.count() >= 6)
					{

						usbDevice.setInformation(USBDisk, m_usbList.at(0), m_usbList.at(1), m_usbList.at(4),
							m_usbList.at(5), m_usbList.at(2), m_usbList.at(3));
					}
					else if (m_usbList.count() >= 3)
					{
						usbDevice.setInformation(USBDisk, m_usbList.at(0), m_usbList.at(1), "",
							"", m_usbList.at(2), "");

					}
					usbDeviceMap.insert({ USBDisk, usbDevice });
					qDebug() << "USB_ADD MapSIZE : " << usbDeviceMap.size();

				}
			}
			if (lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
			{
				PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
				QString strname = QString::fromWCharArray(pDevInf->dbcc_name);
				//��U����Ϣ������ȡ
				if (!strname.isEmpty() && strname.length() > 10)
				{
					if (strname.contains("USBSTOR"))
					{
						QStringList listAll = strname.split('#');
						QStringList listInfo = listAll.at(1).split('&');
						m_usbList.append(listInfo.at(1).mid(4));    //�豸������ 3
						m_usbList.append(listInfo.at(2).mid(5));    //�豸�ͺ� 4
						m_usbList.append(listInfo.at(3).mid(4));    //�豸�汾 5
					}
					else
					{
						m_usbList.clear();
						QStringList listAll = strname.split('#');
						QStringList listID = listAll.at(1).split('&');
						m_usbList.append(listID.at(0).right(4));    //vid 0
						m_usbList.append(listID.at(1).right(4));    //pid 1
						m_usbList.append(listAll.at(2));            //�豸���к� 2
						QString qstrName = m_usbList.at(2);
						//++m_sCameraDeviceIndex;
						/*m_mpCameraDevice.insert({ qstrName, m_sCameraDeviceIndex });
						emit IsCameraUSB(true, qstrName,m_mpCameraDevice[qstrName]);*/
					}
				}

			}
		}
		break;
		case DBT_DEVICEREMOVECOMPLETE:
		{
			InitCamera();//���camera
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv->dbcv_flags == 0)
				{
					QString USBDisk = QString(this->FirstDriveFromMask(lpdbv->dbcv_unitmask));
					qDebug() << "USB_Removed is : " << USBDisk;
					if (usbDeviceMap.find(USBDisk) != usbDeviceMap.end())
					{
						usbDeviceMap.erase(USBDisk);
					}
					qDebug() << "USB_Remov MapSIZE : " << usbDeviceMap.size();
				}
			}
			if (lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
			{
				PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
				QString strname = QString::fromWCharArray(pDevInf->dbcc_name);
				//��U����Ϣ������ȡ
				if (!strname.isEmpty() && strname.length() > 10)
				{
					if (strname.contains("USBSTOR"))
					{
						QStringList listAll = strname.split('#');
						QStringList listInfo = listAll.at(1).split('&');
						m_usbList.append(listInfo.at(1).mid(4));    //�豸������ 3
						m_usbList.append(listInfo.at(2).mid(5));    //�豸�ͺ� 4
						m_usbList.append(listInfo.at(3).mid(4));    //�豸�汾 5
					}
					else
					{
						if (m_mpCameraDevice.empty())
						{
							return false;
						}
						m_usbList.clear();
						QStringList listAll = strname.split('#');
						QStringList listID = listAll.at(1).split('&');
						m_usbList.append(listID.at(0).right(4));    //vid 0
						m_usbList.append(listID.at(1).right(4));    //pid 1
						m_usbList.append(listAll.at(2));            //�豸���к� 2
						QString qstrUSBName = m_usbList.at(2);
						//emit IsCameraUSB(false, qstrUSBName, m_mpCameraDevice[qstrUSBName]);
						//if (m_sCameraDeviceIndex != 0)
						//{
						//	--m_sCameraDeviceIndex;
						//}
						//if (m_mpCameraDevice.find(qstrUSBName) != m_mpCameraDevice.end())//�ж�map���Ƿ���ڸ��豸
						//{
						//	if (!m_mpImageView.empty())
						//	{
						//		DestroyImageView(m_mpCameraDevice[qstrUSBName]);
						//		for (auto iter = m_mpImageView.begin(); iter != m_mpImageView.end(); ++iter)
						//		{
						//			if (iter->second.first > m_mpCameraDevice.find(qstrUSBName)->second)
						//			{
						//				--iter->second.first;
						//			}
						//		}
						//	}

						//	for (auto iter = m_mpCameraDevice.begin(); iter != m_mpCameraDevice.end(); ++iter)//����map
						//	{
						//		if (iter->second > m_mpCameraDevice.find(qstrUSBName)->second)
						//		{
						//			--iter->second;//���Ƴ��豸����豸nIndexȫ���ݼ�1
						//		}
						//	}
						//	m_mpCameraDevice.erase(m_mpCameraDevice.find(qstrUSBName));//�Ƴ����豸
						//}
					}
				}
			}
			break;
		}
		}
	}
	return false;
}

bool AngstrongDemo::InitCamera()
{
	bool bReturn = false;
	do 
	{
		static int m_sCameraDeviceIndex = -1;              //�ֲ���̬�����������洢USBCamera���
		int cameraNum = CCameraDS::CameraCount();
		if (cameraNum <= 0)
		{
			IsCameraUSB(false, QString(""), -1);
			m_mpCameraDevice.clear();
			break;
		}
		char camName[100];
		for (int i = 0; i < cameraNum; i++)
		{
			CCameraDS::CameraName(i, camName, 100);
			std::string cN(camName);
			if (cN.find("UVC") != std::string::npos)
			{
				IsCameraUSB(true, QString::fromStdString(cN) + QString::number(i), i);
				//++m_sCameraDeviceIndex;
				m_mpCameraDevice.insert({ QString::fromStdString(cN)+QString::number(i), i });
			}
		}

		bReturn = true;
	} while (false);

	return bReturn;
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
	addDockWidget(Qt::RightDockWidgetArea, m_dock_output);
	m_dock_output->setWidget(&m_OutputView);
	//����SaveDataͣ������
	QDockWidget *m_dock_savedata = new QDockWidget(tr("SaveData"));
	m_dock_savedata->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_output->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::BottomDockWidgetArea);
	m_dock_savedata->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::RightDockWidgetArea, m_dock_savedata);
	m_dock_savedata->setWidget(&m_SaveData);
	//����DisplayViewͣ������
	QDockWidget *m_dock_display = new QDockWidget(tr("Display"));
	m_dock_display->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_output->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::BottomDockWidgetArea);
	m_dock_display->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::RightDockWidgetArea, m_dock_display);
	m_dock_display->setWidget(&m_DispView);
	//����XMViewͣ������
	QDockWidget *m_dock_xm = new QDockWidget(tr("XM"));
	m_dock_xm->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_output->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::BottomDockWidgetArea);
	m_dock_xm->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::RightDockWidgetArea, m_dock_xm);
	m_dock_xm->setWidget(&m_XMView);
	//����ImageViewͣ������
	//QDockWidget *m_dock_imageview = new QDockWidget(tr("ImageView"));
	//m_dock_imageview->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	////m_dock_imageview->setAllowedAreas(Qt::LeftDockWidgetArea/* | Qt::BottomDockWidgetArea*/);
	//m_dock_imageview->setAllowedAreas(Qt::AllDockWidgetAreas);
	//addDockWidget(Qt::LeftDockWidgetArea, m_dock_imageview);
	//if (!m_vec_spImageView.empty())
	//{
	//	if (m_vec_spImageView.at(0))
	//	{
	//		m_dock_imageview->setWidget(m_vec_spImageView.at(0).get());
	//	}
	//}

	//�������ڲ���
	tabifyDockWidget(m_dock_paramlist, m_dock_savedata);
	tabifyDockWidget(m_dock_paramlist, m_dock_xm);
	tabifyDockWidget(m_dock_output, m_dock_display);
}

void AngstrongDemo::AddToolBar()
{
	/*QToolBar* pToolBar = ui.mainToolBar;

	QAction* pActionC = new QAction(QIcon(QPixmap(":/AngstrongDemo/image_ico/Option_normal.png")), "��");

	pToolBar->addAction(pActionC);*/
}

void AngstrongDemo::BuildConnect()
{
	connect(this, SIGNAL(IsCameraUSB(bool, QString, int)), &m_CameraView, SLOT(DetectCameraUSB(bool, QString, int)));
	connect(&m_CameraView, SIGNAL(SelectCamera(QString,int)), this, SLOT(ShowImageView(QString,int)));
	connect(&m_SaveData, SIGNAL(SendSaveDataStatus(bool, int, int,QString)), m_pMainImageView, SLOT(ReceiveSaveDataStatus(bool, int, int,QString)));
	connect(&m_ParamView, SIGNAL(SendCameraStatus(ECameraStatus)), m_pMainImageView, SLOT(ReceiveCameraStatus(ECameraStatus)));
	connect(&m_ParamView, SIGNAL(SendCreateAvgArea(int, bool)), m_pMainImageView->ui->m_gView_ImageView, SLOT(ReceiveCreateAvgArea(int, bool)));
	connect(m_pMainImageView, SIGNAL(SendCameraStatus(ECameraStatus)), &m_ParamView, SLOT(ReceiveCameraStatus(ECameraStatus)));
	connect(m_pMainImageView, SIGNAL(SendSaveImageInfo(QString)), &m_SaveData, SLOT(ReceiveSaveImageInfo(QString)));
	connect(m_pMainImageView->m_pCamera, SIGNAL(SendLocationDepth(int, int, float)), &m_DispView, SLOT(ReceiveLocationDepth(int, int, float)));
	connect(m_pMainImageView->ui->m_gView_ImageView, SIGNAL(SendImageGray(int, int, int)), &m_DispView, SLOT(ReceiveImageGray(int, int, int)));
	connect(m_pMainImageView->m_pCamera, SIGNAL(SendAvgDepth(float,float)), &m_DispView, SLOT(ReceiveAvgDepth(float, float)));
}

void AngstrongDemo::registerDevice()
{
	const GUID GUID_DEVINTERFACE_LIST[] =
	{
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } } 
	};

	HDEVNOTIFY hDevNotify;
	DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
	ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
	NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];

		hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(), &NotifacationFiler, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (!hDevNotify)
		{
			//qDebug() << QStringLiteral("ע��ʧ�ܣ�") <<endl;
			qDebug() << QStringLiteral("error") << endl;
		}
	}
}

char AngstrongDemo::FirstDriveFromMask(ULONG unitmask)
{
	char i;
	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}

void AngstrongDemo::DestroyImageView(int nIndex)
{
	if (nIndex < 0)
	{
		return;
	}
	for (auto iter = m_mpImageView.begin(); iter != m_mpImageView.end(); ++iter)
	{
		if (iter->second.first == nIndex)
		{
			iter->second.second->close();
			delete iter->second.second;
			iter->second.second = nullptr;
			m_mpImageView.erase(iter);
			break;
		}
	}
}

void AngstrongDemo::ShowImageView(QString qstrName,int nIndex)
{
	if (nIndex < 0)
	{
		return;
	}
	if (m_mpImageView.empty())
	{
		m_mpImageView.insert({ qstrName, {nIndex,new ImageView()} });
		QDockWidget *m_dock_imageview = new QDockWidget(tr("ImageView"));
		m_dock_imageview->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
		m_dock_imageview->setAllowedAreas(Qt::AllDockWidgetAreas);
		addDockWidget(Qt::LeftDockWidgetArea, m_dock_imageview);
		if (m_mpImageView[qstrName].second)
		{
			m_dock_imageview->setWidget(m_mpImageView[qstrName].second);
		}
	}
	else
	{
		auto iter = m_mpImageView.begin();
		for (; iter != m_mpImageView.end(); ++iter)
		{
			if (iter->second.first == nIndex)
			{
				break;
			}
		}
		if (iter == m_mpImageView.end())
		{
			m_mpImageView.insert({ qstrName, {nIndex,new ImageView() } });
		}
	}

	if (m_mpImageView[qstrName].second)
	{
		QString strIndex = QString::number(nIndex);
		QString strTitle("ImageView");
		m_mpImageView[qstrName].second->setWindowTitle(strTitle + "[" + strIndex + "]");
		m_mpImageView[qstrName].second->show();
	}
}