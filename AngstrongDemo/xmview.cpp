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

#define HOLDER_OTP_SIZE 512

XMView::XMView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XMView)
{
    ui->setupUi(this);

	//初始化界面
	ui->m_lineEdit_filepath->setEnabled(false);
	BuildConnect();
	log_msg_ = "";
}

XMView::~XMView()
{
    delete ui;
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
	if (cc.upload_data_to_flash_test(data, HOLDER_OTP_SIZE) == 0)
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

void XMView::BuildConnect()
{
	connect(ui->m_btn_chose_file, SIGNAL(clicked(bool)), this, SLOT(on_choose_clicked()));
	connect(ui->m_btn_upload, SIGNAL(clicked(bool)), this, SLOT(on_upload_cliecked()));
}
