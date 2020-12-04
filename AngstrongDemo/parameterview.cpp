#include "parameterview.h"
#include "ui_parameterview.h"
#include <QDebug>

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
	ui->m_combo_display_mode->setCurrentIndex(EDisplayMode_IR_Depth_RGB);
}

ParameterView::~ParameterView()
{
    delete ui;
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
	qDebug() << "highlight";
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
		ui->m_btn_open->setEnabled(false);
		ui->m_btn_close->setEnabled(false);
		ui->m_btn_live->setEnabled(false);
		ui->m_btn_pause->setEnabled(false);
		ui->m_btn_stop->setEnabled(false);
	}
		break;
	case ECameraStatus_Open:
	{
		ui->m_btn_open->setEnabled(false);
		ui->m_btn_close->setEnabled(true);
		ui->m_btn_live->setEnabled(true);
		ui->m_btn_pause->setEnabled(true);
		ui->m_btn_stop->setEnabled(true);
	}
		break;
	case ECameraStatus_Close:
	{
		ui->m_btn_open->setEnabled(true);
		ui->m_btn_close->setEnabled(false);
		ui->m_btn_live->setEnabled(false);
		ui->m_btn_pause->setEnabled(false);
		ui->m_btn_stop->setEnabled(false);
	}
		break;
	case ECameraStatus_Live:
	{
		ui->m_btn_live->setEnabled(false);
		ui->m_btn_pause->setEnabled(true);
		ui->m_btn_stop->setEnabled(true);
	}
		break;
	case ECameraStatus_Pause:
	{
		ui->m_btn_live->setEnabled(true);
		ui->m_btn_pause->setEnabled(false);
	}
		break;
	case ECameraStatus_Stop:
	{
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
	if (bUSB)
	{
		ui->m_combo_camera_list->addItem(qstrUSBName);
	}
	else
	{
		int camera_list_count = ui->m_combo_camera_list->count();
		for (size_t i = 0; i < camera_list_count; i++)
		{
			ui->m_combo_camera_list->clear();
		}
	}
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
