#pragma execution_character_set("utf-8")
#include <QTextCodec>
#include <QFileDialog>
#include <stdio.h>
#include <thread>
#include <string>
#include "xmview.h"
#include "otp_interface.h"
#include "ui_xmview.h"
#include "logmanager.h"
#include "HalerThread.h"

#define HOLDER_OTP_SIZE 512

enum EThreadSequence
{
	EThreadSequence_pContext_ReadCommand = 0,
	EThreadSequence_Count,
};

XMView::XMView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XMView)
{
    ui->setupUi(this);

	//初始化界面
	ui->m_lineEdit_filepath->setEnabled(false);
	ui->m_lineEdit_write_command->setText("ARG WRITE:DIST=1");
	ui->m_lineEdit_read_command->setText("ARG READ:DIST");
	BuildConnect();
	CreateHalerThread();
	log_msg_ = "";
}

XMView::~XMView()
{
    delete ui;
	quite_program = true;
	Sleep(500);
	CHalerThread::Terminate(EThreadSequence_pContext_ReadCommand);
}

void XMView::on_choose_clicked()
{
	QString filename;
	filename = QFileDialog::getOpenFileName(this, tr("选择文件"), "", tr("文件 (*.bin *.txt *.xml *.* )"));
	if (filename.isEmpty())
	{
		ui->m_lab_display_msg->setText("Error:Invalid file!");
		log_msg_ = "[xmview] Error:Invalid file!";
		LogManager::Write(log_msg_);
		return;
	}
	
	ui->m_lineEdit_filepath->setText(filename);
	ui->m_lab_display_msg->setText("file:" + ui->m_lineEdit_filepath->text());
	log_msg_ = "[xmview] file:" +ui->m_lineEdit_filepath->text().toStdString();
	LogManager::Write(log_msg_);
}

void XMView::on_upload_cliecked()
{
	unsigned char* data = new unsigned char[HOLDER_OTP_SIZE];
	FILE* fp = fopen(ui->m_lineEdit_filepath->text().toLocal8Bit().data(), "rb");
	if (!fp)
	{
		ui->m_lab_display_msg->setText("fail to open the file!");
		log_msg_ = "[xmview] fail to open the file!";
		LogManager::Write(log_msg_);
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
		return;
	}
	fread(data, HOLDER_OTP_SIZE, 1, fp);
	fclose(fp);

	com_controler cc;
	int nCom = 0;
	
	if (ui->m_lineEdit_com->text().isEmpty())
	{
		ui->m_lab_display_msg->setText("Invalid port number!");
		log_msg_ = "[xmview] Invalid port number!";
		LogManager::Write(log_msg_);
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
		return;
	}
	else
	{
		nCom = ui->m_lineEdit_com->text().toInt();
	}

	if (cc.init_comm(nCom) != 0)
	{
		ui->m_lab_display_msg->setText("fail to open the port!");
		log_msg_ = "[xmview] fail to open the port!";
		LogManager::Write(log_msg_);
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
		return;
	}

	ui->m_lab_display_msg->setText("uploading...");
	if (cc.upload_data_to_flash(data, HOLDER_OTP_SIZE) == 0)
	{
		ui->m_lab_display_msg->setText("upload completed!");
		log_msg_ = "[xmview] upload completed!";
	}
	else
	{
		ui->m_lab_display_msg->setText("failed to upload!");
		log_msg_ = "[xmview] failed to upload!";
	}
	cc.close_comm();
	LogManager::Write(log_msg_);
	if (data)
	{
		delete[] data;
		data = nullptr;
	}
}

void XMView::on_write_command_clicked()
{
	CHalerThread::Resume(EThreadSequence_pContext_ReadCommand);
}

void XMView::on_read_command_clicked()
{
	CHalerThread::Suspend(EThreadSequence_pContext_ReadCommand);
}

void XMView::BuildConnect()
{
	connect(ui->m_btn_chose_file, SIGNAL(clicked(bool)), this, SLOT(on_choose_clicked()));
	connect(ui->m_btn_upload, SIGNAL(clicked(bool)), this, SLOT(on_upload_cliecked()));
	connect(ui->m_btn_write_arg_command, SIGNAL(clicked(bool)), this, SLOT(on_write_command_clicked()));
	connect(ui->m_btn_read_arg_command, SIGNAL(clicked(bool)), this, SLOT(on_read_command_clicked()));
}

void XMView::CreateHalerThread()
{
	CreateHalerThreadContext(pContext_ReadCommand)
	{
		while (!quite_program)
		{
			int nCom = 0;
			if (ui->m_lineEdit_com->text().isEmpty())
			{
				ui->m_lab_display_msg->setText("Invalid port number!");
				log_msg_ = "[xmview] Invalid port number!";
				LogManager::Write(log_msg_);
				continue;
			}
			else
			{
				nCom = ui->m_lineEdit_com->text().toInt();
			}
			std::unique_lock<std::mutex> locker(mutex_);
			com_controler cc;
			if (cc.init_comm(nCom) != 0)
			{
				ui->m_lab_display_msg->setText("fail to open the port!");
				log_msg_ = "[xmview] fail to open the port!";
				LogManager::Write(log_msg_);
				continue;
			}

			std::string write_info("");
			std::string read_info("");
			std::string write_command = ui->m_lineEdit_write_command->text().toStdString();
			std::string read_command = ui->m_lineEdit_read_command->text().toStdString();
			cc.write_comm(write_command, write_info);
			cc.write_comm(read_command, read_info);
			if (ui->m_checkBox_dispaly_write_data->isChecked())
			{
				LogManager::Write("write:"+write_info);
			}
			if (ui->m_checkBox_display_read_data->isChecked())
			{
				LogManager::Write("read:"+read_info);
			}
			cc.close_comm();
			locker.unlock();
			Sleep(2);
		}
	};
	CHalerThread::Run(pContext_ReadCommand, EThreadSequence_pContext_ReadCommand,true);
}
