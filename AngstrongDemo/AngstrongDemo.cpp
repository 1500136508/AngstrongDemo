#pragma execution_character_set("utf-8")

#include <windows.h>
#include <QDebug>
#include <dbt.h>
#include <QTimer>
#include <QPalette>
#include <QString>
#include "AngstrongDemo.h"

AngstrongDemo::AngstrongDemo(QWidget *parent)
	: QMainWindow(parent)
{
	registerDevice();//ע��USB����¼�
	ui.setupUi(this);

	//��ʼ��
	m_mpCameraDevice.clear();
	usbDeviceMap.clear();
	m_mpCameraDevice.clear();

	BuildConnect();

	//setCentralWidget(&m_ImageView);
	CreateDockWindow();
	AddToolBar();
}

AngstrongDemo::~AngstrongDemo()
{
	if (!m_mpImageView.empty())
	{
		for (auto mp : m_mpImageView)
		{
			if (mp.second)
			{
				mp.second->close();
				delete mp.second;
				mp.second = nullptr;
			}
		}
		m_mpImageView.clear();
	}
}

bool AngstrongDemo::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG* msg = reinterpret_cast<MSG*>(message);
	int msgType = msg->message;
	static unsigned int m_sCameraDeviceIndex = 0;//�ֲ���̬�����������洢USBCamera���
	if (msgType == WM_DEVICECHANGE)
	{
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
						++m_sCameraDeviceIndex;
						m_mpCameraDevice.insert({ m_usbList.at(2),m_sCameraDeviceIndex });
						emit IsCameraUSB(true,m_usbList.at(2),m_mpCameraDevice);
					}
				}

			}
		}
		break;
		case DBT_DEVICEREMOVECOMPLETE:
		{
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
						emit IsCameraUSB(false, m_usbList.at(2), m_mpCameraDevice);
						if (m_sCameraDeviceIndex != 0)
						{
							--m_sCameraDeviceIndex;
						}
						if (m_mpCameraDevice.find(m_usbList.at(2)) != m_mpCameraDevice.end())//�ж�map���Ƿ���ڸ��豸
						{
							for (auto iter = m_mpCameraDevice.begin(); iter != m_mpCameraDevice.end();++iter)//����map
							{
								if (iter->second > m_mpCameraDevice.find(m_usbList.at(2))->second)
								{
									--iter->second;//���Ƴ��豸����豸nIndexȫ���ݼ�1
								}
							}
							if (!m_mpImageView.empty())
							{
								auto iter1 = m_mpImageView.begin();
								int nTempIndex = -1;
								for (; iter1 != m_mpImageView.end(); ++iter1)
								{
									if (iter1->second->GetImageViewIndex() == m_mpCameraDevice.find(m_usbList.at(2))->second)
									{
										break;
									}
								}
								if (iter1 != m_mpImageView.end())
								{
									DestroyImageView(iter1->second->GetImageViewIndex());
								}
								for (auto it = m_mpImageView.begin(); it != m_mpImageView.end(); ++it)
								{
									if (it->second->GetImageViewIndex() > nTempIndex)
									{
										int nTemp = it->second->GetImageViewIndex();
										it->second->SetImageViewIndex(nTemp - 1);//���ڶ��ڱ�ʶ�ݼ�
									}
								}
							}
							m_mpCameraDevice.erase(m_mpCameraDevice.find(m_usbList.at(2)));//�Ƴ����豸
						}
					}
				}
			}
			break;
		}
		}
	}
	return false;
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
	//addDockWidget(Qt::LeftDockWidgetArea, m_dock_imageview);
	//if (!m_vec_spImageView.empty())
	//{
	//	if (m_vec_spImageView.at(0))
	//	{
	//		m_dock_imageview->setWidget(m_vec_spImageView.at(0).get());
	//	}
	//}
}

void AngstrongDemo::AddToolBar()
{
	QToolBar* pToolBar = ui.mainToolBar;

	QAction* pActionC = new QAction(QIcon(QPixmap(":/AngstrongDemo/image_ico/Option_normal.png")), "��");

	pToolBar->addAction(pActionC);
}

void AngstrongDemo::BuildConnect()
{
	/*qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::string>("std::string");*/
	connect(this, SIGNAL(IsCameraUSB(bool, QString, std::map<QString, unsigned>)), &m_CameraView, SLOT(DetectCameraUSB(bool, QString, std::map<QString, unsigned>)));
	connect(&m_CameraView, SIGNAL(SelectCamera(int)), this, SLOT(ShowImageView(int)));
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
		if (iter->second->GetImageViewIndex() == nIndex)
		{
			iter->second->close();
			delete iter->second;
			iter->second = nullptr;
			m_mpImageView.erase(iter);
			break;
		}
	}
	/*if (m_mpImageView.find(nIndex) != m_mpImageView.end())
	{
		if (m_mpImageView.find(nIndex)->second)
		{
			m_mpImageView[nIndex]->close();
			delete m_mpImageView[nIndex];
			m_mpImageView[nIndex] = nullptr;
		}
		m_mpImageView.erase(m_mpImageView.find(nIndex));
	}*/
}

void AngstrongDemo::ShowImageView(int nIndex)
{
	if (nIndex < 0)
	{
		return;
	}
	if (m_mpImageView.empty())
	{
		m_mpImageView.insert({ nIndex,new ImageView() });
	}
	else
	{
		auto iter = m_mpImageView.begin();
		for (; iter != m_mpImageView.end(); ++iter)
		{
			if (iter->second->GetImageViewIndex() == nIndex)
			{
				break;
			}
		}
		if (iter == m_mpImageView.end())
		{
			m_mpImageView.insert({ nIndex,new ImageView() });
			m_mpImageView[nIndex]->SetImageViewIndex(nIndex);//���ô��ڵı�ʶ�������㴰�ڹ���
			QString strIndex = QString::number(nIndex);
			QString strTitle("ImageView");
			m_mpImageView[nIndex]->setWindowTitle(strTitle + "[" + strIndex + "]");
		}
	}

	m_mpImageView[nIndex]->SetImageViewIndex(nIndex);//���ô��ڵı�ʶ�������㴰�ڹ���
	QString strIndex = QString::number(nIndex);
	QString strTitle("ImageView");
	m_mpImageView[nIndex]->setWindowTitle(strTitle + "[" + strIndex + "]");
	m_mpImageView[nIndex]->show();
}