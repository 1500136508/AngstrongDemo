#include "parameterview.h"
#include "ui_parameterview.h"

ParameterView::ParameterView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterView)
{
    ui->setupUi(this);

	//初始化界面
	ui->m_btn_open->setEnabled(true);
	ui->m_btn_close->setEnabled(false);
	ui->m_btn_live->setEnabled(false);
	ui->m_btn_pause->setEnabled(false);
	ui->m_btn_stop->setEnabled(false);

	//初始信号槽
	BuildConnect();
}

ParameterView::~ParameterView()
{
    delete ui;
}

void ParameterView::on_open_clicked()
{
	emit SendCameraStatus(ECameraStatus_Open);
	emit SendCameraStatus(ECameraStatus_Live);
}

void ParameterView::on_close_clicked()
{
	emit SendCameraStatus(ECameraStatus_Close);
}

void ParameterView::on_live_clicked()
{
	emit SendCameraStatus(ECameraStatus_Live);
}

void ParameterView::on_pause_clicked()
{
	emit SendCameraStatus(ECameraStatus_Pause);
}

void ParameterView::on_stop_clicked()
{
	emit SendCameraStatus(ECameraStatus_Stop);
}

void ParameterView::ReceiveCameraStatus(ECameraStatus eStatus)
{
	//return;//目前关闭此功能
	switch (eStatus)
	{
	case ECameraStatus_Unknow:
	{

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

void ParameterView::BuildConnect()
{
	connect(ui->m_btn_open, SIGNAL(clicked()), this, SLOT(on_open_clicked()));
	connect(ui->m_btn_close, SIGNAL(clicked()), this, SLOT(on_close_clicked()));
	connect(ui->m_btn_live, SIGNAL(clicked()), this, SLOT(on_live_clicked()));
	connect(ui->m_btn_pause, SIGNAL(clicked()), this, SLOT(on_pause_clicked()));
	connect(ui->m_btn_stop, SIGNAL(clicked()), this, SLOT(on_stop_clicked()));
}
