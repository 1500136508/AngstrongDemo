#pragma execution_character_set("utf-8")
#include "setupview.h"
#include "ui_setupview.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "modelparameter.h"
#include "definition_camera.h"

const int IMAGEVIEWCOUNT = 2;

SetupView::SetupView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupView)
{
    ui->setupUi(this);
	for (size_t i = 0; i < IMAGEVIEWCOUNT; ++i)
	{
		ui->m_combo_image_veiw_count->addItem(QString::number(i + 1));
	}
	for (size_t i = 0; i < EDisplayMode_Count; ++i)
	{
		switch (i)
		{
		case EDisplayMode_Unknow:
			break;
		case EDisplayMode_IR_Depth_RGB:
		{
			ui->m_combo_dispaly_mode->addItem("IR_Depth_RGB");
		}
		break;
		case EDisplayMode_IR:
		{
			ui->m_combo_dispaly_mode->addItem("IR");
		}
		break;
		case EDisplayMode_Depth:
		{
			ui->m_combo_dispaly_mode->addItem("Depth");
		}
		break;
		case EDisplayMode_RGB:
		{
			ui->m_combo_dispaly_mode->addItem("RGB");
		}
		break;
		case EDisplayMode_IR_Depth_RGBAddDepth:
		{
			ui->m_combo_dispaly_mode->addItem("IR_Depth_RGBAddDepth");
		}
		break;
		default:
			break;
		}
	}
	BuildConnect();
	InitParameter();
	//嵌入qss进行美化
	QFile file("black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	setStyleSheet(stylesheet);
}

SetupView::~SetupView()
{
    delete ui;
}

void SetupView::BuildConnect()
{
	connect(ui->m_btn_save, &QPushButton::clicked, this, &SetupView::on_save_parameter_clicked);
	connect(ui->m_btn_ok, &QPushButton::clicked, this, &SetupView::on_ok_clicked);
}

void SetupView::InitParameter()
{
	if (!ModelParameter::IsOpen())
	{
		if (!ModelParameter::Open("model/parameter.ini"))
		{
			ui->m_combo_image_veiw_count->setCurrentIndex(0);
			ui->m_combo_dispaly_mode->setCurrentIndex(0);
		}
	}
	ui->m_combo_image_veiw_count->setCurrentIndex(ModelParameter::get_int_value("AngstrongDemo", "image_view_count"));
	ui->m_combo_dispaly_mode->setCurrentIndex(ModelParameter::get_int_value("AngstrongDemo", "image_display_mode"));
}

void SetupView::on_ok_clicked()
{
	Save();
	this->close();
}

void SetupView::Save()
{
	if (!ModelParameter::IsOpen())
	{
		ModelParameter::Open("model/parameter.ini");
	}
	int image_view_count = ui->m_combo_image_veiw_count->currentIndex();
	int display_mode = ui->m_combo_dispaly_mode->currentIndex();
	ModelParameter::set_int_value("AngstrongDemo", "image_view_count", image_view_count);
	ModelParameter::set_int_value("AngstrongDemo", "image_display_mode", display_mode);
	ModelParameter::Save();
}

void SetupView::on_save_parameter_clicked()
{
	Save();
	QMessageBox message_box;
	message_box.setStyleSheet(stylesheet);
	message_box.setWindowTitle("设置");
	message_box.setWindowIcon(QIcon("camera.ico"));
	message_box.setInformativeText("参数保存成功！");
	//message_box.setInformativeText(NULL, "参数设置", "参数保存成功！", QMessageBox::Yes, QMessageBox::Yes);
	message_box.exec();
}