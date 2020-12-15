#pragma execution_character_set("utf-8")
#include "parameterview.h"
#include "ui_parameterview.h"
#include <windows.h>
#include <iterator>
#include <tchar.h>
#include <QDebug>
#include <logmanager.h>
#include "modelparameter.h"
#include "HalerThread.h"
#include "otp_interface.h"
#include "EnumSerial.h"
#include "definition_thread.h"

ParameterView::ParameterView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterView)
{
    ui->setupUi(this);

	//初始信号槽
	BuildConnect();
	//初始化界面
	ui->m_btn_open->setEnabled(true);
	ui->m_btn_close->setEnabled(false);
	ui->m_btn_live->setEnabled(false);
	ui->m_btn_pause->setEnabled(false);
	ui->m_btn_stop->setEnabled(false);
	ui->m_combo_display_mode->addItem("IR_Depth_RGB");
	ui->m_combo_display_mode->addItem("IR");
	ui->m_combo_display_mode->addItem("Depth");
	ui->m_combo_display_mode->addItem("RGB");
	ui->m_combo_display_mode->addItem("IR_Depth_RGBAddDepth");
	if (!ModelParameter::IsOpen())
	{
		if (!ModelParameter::Open("model/parameter.ini"))
		{
			ui->m_combo_display_mode->setCurrentIndex(EDisplayMode_IR_Depth_RGB);
		}
	}
	ui->m_combo_display_mode->setCurrentIndex(ModelParameter::get_int_value("AngstrongDemo","image_display_mode"));
	FindAllPort();
	CreateHalerThread();
}

ParameterView::~ParameterView()
{
    delete ui;
	quite_program_ = true;
	Sleep(500);
	CHalerThread::Terminate(EThreadSequence_pContext_ReadCommand);
}

void ParameterView::on_open_clicked()
{
	current_camera_index_ = ui->m_combo_camera_list->currentIndex();
	emit SendCameraStatus(ECameraStatus_Open,current_camera_index_);
	emit SendCameraStatus(ECameraStatus_Live, current_camera_index_);
}

void ParameterView::on_close_clicked()
{
	current_camera_index_ = ui->m_combo_camera_list->currentIndex();
	emit SendCameraStatus(ECameraStatus_Close, current_camera_index_);
}

void ParameterView::on_live_clicked()
{
	current_camera_index_ = ui->m_combo_camera_list->currentIndex();
	emit SendCameraStatus(ECameraStatus_Live, current_camera_index_);
}

void ParameterView::on_pause_clicked()
{
	current_camera_index_ = ui->m_combo_camera_list->currentIndex();
	emit SendCameraStatus(ECameraStatus_Pause, current_camera_index_);
}

void ParameterView::on_stop_clicked()
{
	current_camera_index_ = ui->m_combo_camera_list->currentIndex();
	emit SendCameraStatus(ECameraStatus_Stop, current_camera_index_);
}

void ParameterView::on_createroi01_clicked()
{
	emit SendCreateAvgArea(1,true);
}

void ParameterView::on_createroi02_clicked()
{
	emit SendCreateAvgArea(2,true);
}

void ParameterView::on_cancelroi01_clicked()
{
	emit SendCreateAvgArea(1, false);
}

void ParameterView::on_cancelroi02_clicked()
{
	emit SendCreateAvgArea(2, false);
}

void ParameterView::on_get_current_camera_id_index_change(int camera_id)
{

	current_camera_index_ = camera_id;
}

void ParameterView::on_get_current_diplay_mode_index_change(int image_display_mode)
{
	emit SendImageDisplayMode((EDisplayMode)image_display_mode);
}

void ParameterView::ReceiveCameraStatus(ECameraStatus eStatus)
{
	switch (eStatus)
	{
	case ECameraStatus_Unknow:
	{

	}
		break;
	case ECameraStatus_NoCamera:
	{
		current_camera_status_ = ECameraStatus_NoCamera;
		ui->m_btn_open->setEnabled(false);
		ui->m_btn_close->setEnabled(false);
		ui->m_btn_live->setEnabled(false);
		ui->m_btn_pause->setEnabled(false);
		ui->m_btn_stop->setEnabled(false);
	}
		break;
	case ECameraStatus_Open:
	{
		current_camera_status_ = ECameraStatus_Open;
		ui->m_btn_open->setEnabled(false);
		ui->m_btn_close->setEnabled(true);
		ui->m_btn_live->setEnabled(true);
		ui->m_btn_pause->setEnabled(true);
		ui->m_btn_stop->setEnabled(true);
		CHalerThread::Resume(EThreadSequence_pContext_ReadCommand);
	}
		break;
	case ECameraStatus_Close:
	{
		ui->m_btn_open->setEnabled(true);
		ui->m_btn_close->setEnabled(false);
		ui->m_btn_live->setEnabled(false);
		ui->m_btn_pause->setEnabled(false);
		ui->m_btn_stop->setEnabled(false);
		current_camera_status_ = ECameraStatus_Close;
		CHalerThread::Suspend(EThreadSequence_pContext_ReadCommand);
	}
		break;
	case ECameraStatus_Live:
	{
		current_camera_status_ = ECameraStatus_Live;
		ui->m_btn_live->setEnabled(false);
		ui->m_btn_pause->setEnabled(true);
		ui->m_btn_stop->setEnabled(true);
	}
		break;
	case ECameraStatus_Pause:
	{
		current_camera_status_ = ECameraStatus_Pause;
		ui->m_btn_live->setEnabled(true);
		ui->m_btn_pause->setEnabled(false);
	}
		break;
	case ECameraStatus_Stop:
	{
		current_camera_status_ = ECameraStatus_Stop;
		ui->m_btn_live->setEnabled(true);
		ui->m_btn_pause->setEnabled(false);
		ui->m_btn_stop->setEnabled(false);
	}
		break;
	default:
		break;
	}
}

void ParameterView::ReceiveAddCameraUSBString(bool bUSB, QString qstrUSBName, int nIndex)
{
	//std::unique_lock<std::mutex> locker(mutex_);
	if (bUSB)
	{
		ui->m_combo_camera_list->addItem(qstrUSBName);
		if (current_camera_status_ == ECameraStatus_NoCamera)
		{
			ui->m_btn_open->setEnabled(true);
		}
	}
	else
	{
		int camera_list_count = ui->m_combo_camera_list->count();
		for (size_t i = 0; i < camera_list_count; i++)
		{
			ui->m_combo_camera_list->clear();
		}
	}
	//locker.unlock();
}

void ParameterView::BuildConnect()
{

	connect(ui->m_btn_open, SIGNAL(clicked()), this, SLOT(on_open_clicked()));
	connect(ui->m_btn_close, SIGNAL(clicked()), this, SLOT(on_close_clicked()));
	connect(ui->m_btn_live, SIGNAL(clicked()), this, SLOT(on_live_clicked()));
	connect(ui->m_btn_pause, SIGNAL(clicked()), this, SLOT(on_pause_clicked()));
	connect(ui->m_btn_stop, SIGNAL(clicked()), this, SLOT(on_stop_clicked()));
	connect(ui->m_btn_roi_01, SIGNAL(clicked()), this, SLOT(on_createroi01_clicked()));
	connect(ui->m_btn_roi_02, SIGNAL(clicked()), this, SLOT(on_createroi02_clicked()));
	connect(ui->m_btn_cancel_roi_01, SIGNAL(clicked()), this, SLOT(on_cancelroi01_clicked()));
	connect(ui->m_btn_cancel_roi_02, SIGNAL(clicked()), this, SLOT(on_cancelroi02_clicked()));
	connect(ui->m_combo_camera_list, SIGNAL(currentIndexChanged(int)), this, SLOT(on_get_current_camera_id_index_change(int)));
	bool ret = connect(ui->m_combo_display_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(on_get_current_diplay_mode_index_change(int)));
}

void ParameterView::FindAllPort()
{
	ui->m_combo_port->clear();
	std::vector<SSerInfo> port_container;
	port_container.clear();
	EnumSerialPorts(port_container);
	for (size_t sz = 0; sz < port_container.size(); ++sz)
	{
		auto start_position = port_container.at(sz).strFriendlyName.find("(");
		auto end_position = port_container.at(sz).strFriendlyName.find(")");
		auto iter_b = port_container.at(sz).strFriendlyName.cbegin()+ start_position;
		auto iter_e = port_container.at(sz).strFriendlyName.cbegin() + end_position;

		std::string port_no_name(iter_b+1,iter_e);
		ui->m_combo_port->addItem(QString::fromStdString(port_no_name));
	}
	if (port_container.size() > 0)
	{
		ui->m_combo_port->setCurrentIndex(0);
	}
}

void ParameterView::CreateHalerThread()
{
	CreateHalerThreadContext(pContext_ReadCommand)
	{
		while (!quite_program_)
		{
			int nCom = 0;
			std::string log_msg_("");
			//std::unique_lock<std::mutex> locker(mutex_);
			com_controler cc;
			std::string port_name = ui->m_combo_port->currentText().toStdString();
			if (port_name.empty())
			{
				log_msg_ = "[paramterview] fail to select the port!";
				//LogManager::Write(log_msg_);
				emit SendXMData("--");
				//locker.unlock();
				continue;
			}
			int M_position = port_name.find("M");
			auto iter_b = port_name.cbegin() + M_position+1;
			auto iter_e = port_name.cend();
			std::string port_no(iter_b, iter_e);
			nCom = std::stoi(port_no);
			if (cc.init_comm(nCom) != 0)
			{
				log_msg_ = "[paramterview] fail to open the port!";
				//LogManager::Write(log_msg_);
				emit SendXMData("error");
				//locker.unlock();
				continue;
			}

			std::string write_info("Failed to write data!");
			std::string read_info("Failed to read data!");
			std::string write_command = "ARG WRITE:DIST=1";
			std::string read_command = "ARG READ:DIST";
			cc.write_comm(write_command, write_info);
			cc.write_comm(read_command, read_info);
			int find_error_position = read_info.find("error");
			if (find_error_position != -1)
			{
				emit SendXMData("error");
				//locker.unlock();
				continue;
			}
			std::size_t find_position = read_info.find("=");
			int read_return_data = read_info.at(find_position + 1) - '0';
			emit SendXMData(QString::number(read_return_data));
			//LogManager::Write(read_info);
			cc.close_comm();
			//locker.unlock();
		}
	};
	CHalerThread::Run(pContext_ReadCommand, EThreadSequence_pContext_ReadCommand, true);
}
