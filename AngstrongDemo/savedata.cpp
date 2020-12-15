#pragma execution_character_set("utf-8")
#include <QFileInfo>
#include <QTextCodec>
#include <QFileDialog>
#include <qDebug>
#include <QDateTime>
#include "savedata.h"
#include "ui_savedata.h"
#include "logmanager.h"

SaveData::SaveData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveData)
{
    ui->setupUi(this);

	//初始化
	m_bIsStart = false;
	m_nSaveCount = 0;
	log_msg_ = "";
	current_save_path_ = "";
	//设置信号槽
	InitButtonArray();
	BuildConnect();
	InitUI();
}

SaveData::~SaveData()
{
    delete ui;
}

void SaveData::BuildConnect()
{
	connect(ui->m_btn_chose_file, SIGNAL(clicked(bool)), this, SLOT(on_chose_file_clicked()));
	connect(ui->m_btn_start, SIGNAL(clicked(bool)), this, SLOT(on_start_clicked()));
	connect(ui->m_btn_stop, SIGNAL(clicked(bool)), this, SLOT(on_stop_clicked()));
	connect(ui->m_combo_grab_model, SIGNAL(currentIndexChanged(int)), this, SLOT(on_model_currentIndexChanged(int)));
	connect(ui->m_groupBox_parameter, SIGNAL(clicked(bool)), this, SLOT(on_save_parameter_changed(bool)));
	connect(ui->m_combo_save_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combo_save_mode_currentIndexChanged(int)));

	for (size_t sz = 0; sz < radio_button_accessory_mode_.size();++sz)
	{
		connect(radio_button_accessory_mode_[sz], SIGNAL(clicked()), this, SLOT(on_get_current_choose_clicked()));
	}
	for (size_t sz = 0; sz < radio_button_distance_mode_.size(); ++sz)
	{
		connect(radio_button_distance_mode_[sz], SIGNAL(clicked()), this, SLOT(on_get_current_choose_clicked()));
	}
	for (size_t sz = 0; sz < radio_button_face_action_mode_.size(); ++sz)
	{
		connect(radio_button_face_action_mode_[sz], SIGNAL(clicked()), this, SLOT(on_get_current_choose_clicked()));
	}
}

void SaveData::on_chose_file_clicked()
{
	QTextCodec *code = QTextCodec::codecForName("GB2312");
	std::string workPath = code->fromUnicode(QFileDialog::getExistingDirectory(this, tr("Open Directory"), " ", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)).data();
//#if DEBUG
//	qDebug() << QString::fromLocal8Bit(workPath.c_str());
//#endif
	ui->m_lineEdit_file->setText(QString::fromLocal8Bit(workPath.c_str()));

	QFileInfo qfi(ui->m_lineEdit_file->text());
	if (qfi.isDir())
	{
		ui->m_lab_display_status->setText("Save Path:" + ui->m_lineEdit_file->text());
		log_msg_ = "[savedata] Save Path:" + ui->m_lineEdit_file->text().toStdString();
	}
	else
	{
		ui->m_lab_display_status->setText("Error:Save Path Error!");
		log_msg_ = "[savedata] Save Path Error!" + ui->m_lineEdit_file->text().toStdString();
	}
	LogManager::Write(log_msg_);
}

void SaveData::on_start_clicked()
{
	m_bIsStart = true;//启动保存图像标志
	m_nSaveCount = ui->m_lineEdit_pfs_counts->text().toInt();//保存图像帧数
	QString savePath = ui->m_lineEdit_file->text();
	if (savePath.toStdString().empty())
	{
		ui->m_lab_display_status->setText("Error:Please choose the correct path!");
		log_msg_ = "[savedata] Failed to Star to save image!";
		LogManager::Write(log_msg_);
		return;
	}
	if (m_nSaveCount == 0)
	{
		ui->m_lab_display_status->setText("Error:Please set the number of pictures saved!");
		log_msg_ = "[savedata] Failed to Star to save image!";
		LogManager::Write(log_msg_);
		return;
	}

	/*QDir dir;
	dir.setPath(QString::fromStdString(savePath + "//rgb"));
	dir.removeRecursively();
	dir.setPath(QString::fromStdString(savePath + "//ir"));
	dir.removeRecursively();
	dir.setPath(QString::fromStdString(savePath + "//depth"));
	dir.removeRecursively();
	dir.setPath(QString::fromStdString(savePath + "//rgb-alone"));
	dir.removeRecursively();
	dir.setPath(QString::fromStdString(savePath + "//pointcloud"));
	dir.removeRecursively();*/

	//建立时间文件夹
	QDateTime dt;
	QTime time;
	QDate date;
	dt.setTime(time.currentTime());
	dt.setDate(date.currentDate());
	QString currentDate = dt.toString("/yyyy-MM-dd");
	QString currentTime = dt.toString("/hh-mm-ss");

	QDir qd;
	savePath = savePath + currentDate + currentTime;
	switch (m_eCurSaveMode)
	{
	case EGrabMode_3Pix:
	{
		qd.mkpath(savePath);
		qd.mkpath(savePath + "//rgb");
		qd.mkpath(savePath + "//ir");
		qd.mkpath(savePath + "//depth");
		//qd.mkpath(savePath + "//rgb-alone");
	}
		break;
	case EGrabMode_4Pix:
	{
		qd.mkpath(savePath);
		qd.mkpath(savePath + "//rgb");
		qd.mkpath(savePath + "//ir");
		qd.mkpath(savePath + "//depth");
		qd.mkpath(savePath + "//pointcloud");
	}
		break;
	default:
		break;
	}
	QString qstrSaveImageInfo = "Star to Save: " + QString::number(0) + "/" + QString::number(m_nSaveCount);
	ui->m_lab_display_status->setText(qstrSaveImageInfo);
	emit SendSaveDataStatus(m_bIsStart, m_eCurSaveMode, m_nSaveCount, savePath);
}

void SaveData::on_stop_clicked()
{
	m_bIsStart = false;
	emit SendSaveDataStatus(m_bIsStart, m_eCurSaveMode, m_nSaveCount, QString("./data"));
}

void SaveData::on_model_currentIndexChanged(int nIndex)
{
	m_eCurSaveMode = (EGrabMode)nIndex;
}

void SaveData::ReceiveSaveImageInfo(QString qstrInfo)
{
	ui->m_lab_display_status->setText(qstrInfo);
}

void SaveData::on_save_parameter_changed(bool is_check)
{
	if (is_check)
	{
		save_data_mode_ = ESaveMode_User;
		ui->m_combo_save_mode->setCurrentIndex(ESaveMode_User);
	}
	else
	{
		save_data_mode_ = ESaveMode_Default;
		ui->m_combo_save_mode->setCurrentIndex(ESaveMode_Default);
	}
}

void SaveData::on_combo_save_mode_currentIndexChanged(int nIndex)
{
	switch (nIndex)
	{
	case ESaveMode_Default:
	{
		ui->m_groupBox_parameter->setChecked(false);
	}
		break;
	case ESaveMode_User:
	{
		ui->m_groupBox_parameter->setChecked(true);
	}
		break;
	default:
		break;
	}
}

void SaveData::on_get_current_choose_clicked()
{
	std::string accessory_path("");
	switch (get_accessory_current_check_id())
	{
	case EAccessroyMode_none:
	{
		accessory_path = "none";
	}
		break;
	default:
		break;
	}
}

void SaveData::InitUI()
{
	m_eCurSaveMode = EGrabMode_3Pix;
	ui->m_combo_grab_model->addItem("三图模式");
	ui->m_combo_grab_model->addItem("四图模式");
	ui->m_combo_grab_model->setCurrentIndex(m_eCurSaveMode);

	save_data_mode_ = ESaveMode_Default;
	ui->m_combo_save_mode->addItem("默认");
	ui->m_combo_save_mode->addItem("自定义");
	ui->m_combo_save_mode->setCurrentIndex(save_data_mode_);

	ui->m_rbtn_acc_none->setChecked(true);
	ui->m_rbtn_distance_300->setChecked(true);
	ui->m_rbtn_face_action_none->setChecked(true);
	ui->m_groupBox_parameter->setCheckable(true);
	ui->m_groupBox_parameter->setChecked(false);
}

void SaveData::InitButtonArray()
{
	radio_button_accessory_mode_.clear();
	radio_button_accessory_mode_.push_back(ui->m_rbtn_acc_none);
	radio_button_accessory_mode_.push_back(ui->m_rbtn_acc_hat);
	radio_button_accessory_mode_.push_back(ui->m_rbtn_acc_glasses);
	radio_button_accessory_mode_.push_back(ui->m_rbtn_acc_colorglasses);
	radio_button_accessory_mode_.push_back(ui->m_rbtn_acc_mask);

	radio_button_distance_mode_.clear();
	radio_button_distance_mode_.push_back(ui->m_rbtn_distance_300);
	radio_button_distance_mode_.push_back(ui->m_rbtn_distance_500);
	radio_button_distance_mode_.push_back(ui->m_rbtn_distance_800);

	radio_button_face_action_mode_.clear();
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_none);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_close_eyes);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_not_normal_eyes);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_open_mouth);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_not_normal_mouth);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_right_eye_block);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_left_eye_block);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_nose_block);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_mouth_block);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_left_face_block);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_right_face_block);
	radio_button_face_action_mode_.push_back(ui->m_rbtn_face_action_chin_block);
}

int SaveData::get_accessory_current_check_id()
{
	int current_check_id = -1;
	for (size_t sz = 0; sz < EAccessroyMode_Count;++sz)
	{
		if (radio_button_accessory_mode_[sz]->isChecked())
		{
			current_check_id = sz;
			break;
		}
	}
	return current_check_id;
}

int SaveData::get_distanct_current_check_id()
{
	int current_check_id = -1;
	for (size_t sz = 0; sz < EDistanceMode_Count; ++sz)
	{
		if (radio_button_distance_mode_[sz]->isChecked())
		{
			current_check_id = sz;
			break;
		}
	}
	return current_check_id;
}

int SaveData::get_face_action_current_check_id()
{
	int current_check_id = -1;
	for (size_t sz = 0; sz < EFaceActionMode_Count; ++sz)
	{
		if (radio_button_face_action_mode_[sz]->isChecked())
		{
			current_check_id = sz;
			break;
		}
	}
	return current_check_id;
}
