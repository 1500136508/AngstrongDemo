#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include <thread>
#include <vector>
#include <memory>
#include <QMap>
#include "ui_AngstrongDemo.h"
#include "imageview.h"
#include "cameraview.h"
#include "parameterview.h"
#include "outputview.h"
#include "usbdevice.h"


class AngstrongDemo : public QMainWindow
{
    Q_OBJECT

public:
    AngstrongDemo(QWidget *parent = Q_NULLPTR);
	virtual ~AngstrongDemo();
protected:
	//Only Qt Framework
	bool nativeEvent(const QByteArray & eventType, void * message, long*result);
private:
	void CreateDockWindow();
	void AddToolBar();
	void BuildConnect();

	//CameraUSB Device
	void registerDevice();
	char FirstDriveFromMask(ULONG unitmask);
private slots:
	void ShowImageView(int nIndex);
signals:
	void IsCameraUSB(bool bUSB);
private:
    Ui::AngstrongDemoClass ui;

	ImageView m_ImageView;
	CameraView m_CameraView;
	ParameterView m_ParamView;
	OutputView m_OutputView;

	ImageView m_testImageView;

	QMap<QString, USBDevice> usbDeviceMap;             //用来存储U盘信息的Map
	QStringList m_usbList;                             //存储U盘中间信息
};
