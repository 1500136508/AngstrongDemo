#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include <thread>
#include <vector>
#include <memory>
#include <map>
#include "ui_AngstrongDemo.h"
#include "imageview.h"
#include "cameraview.h"
#include "parameterview.h"
#include "outputview.h"
#include "usbdevice.h"
#include "savedata.h"
#include "displayview.h"
#include "xmview.h"

class AngstrongDemo : public QMainWindow
{
    Q_OBJECT

public:
    AngstrongDemo(QWidget *parent = Q_NULLPTR);
	virtual ~AngstrongDemo();
protected:
	//Only Qt Framework
	bool nativeEvent(const QByteArray & eventType, void * message, long*result);
	bool InitCamera();
private:
	void CreateDockWindow();
	void AddToolBar();
	void BuildConnect();

	//CameraUSB Device
	void registerDevice();
	char FirstDriveFromMask(ULONG unitmask);
	void DestroyImageView(int nIndex);
private slots:
	void ShowImageView(QString qstrUSBName, int nIndex);
signals:
	void IsCameraUSB(bool bUSB, QString qstrUSBName, int nIndex);
private:
    Ui::AngstrongDemoClass ui;

	ImageView *m_pMainImageView;
	std::map<QString,std::pair<unsigned, ImageView*>> m_mpImageView;
	CameraView m_CameraView;
	ParameterView m_ParamView;
	OutputView m_OutputView;
	SaveData m_SaveData;
	DisplayView m_DispView;
	XMView m_XMView;

	std::map<QString, unsigned> m_mpCameraDevice;
	std::map<QString, USBDevice> usbDeviceMap;             //�����洢U����Ϣ��Map
	QStringList m_usbList;                                //�洢U���м���Ϣ

	QString stylesheet;							// QSS��ʽ��
};
