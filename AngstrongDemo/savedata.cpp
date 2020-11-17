#pragma execution_character_set("utf-8")
#include <QFileInfo>
#include <QTextCodec>
#include <QFileDialog>
#include <qDebug>
#include "savedata.h"
#include "ui_savedata.h"

SaveData::SaveData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveData)
{
    ui->setupUi(this);

	//初始化
	m_bIsStart = false;
	m_eCurSaveMode = ESaveMode_3Pix;
	m_nSaveCount = 0;
	//初始化窗口数据
	ui->m_combo_model->addItem("三图模式");
	ui->m_combo_model->addItem("四图模式");
	ui->m_combo_model->setCurrentIndex(m_eCurSaveMode);

	//设置信号槽
	BuildConnect();
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
	connect(ui->m_combo_model, SIGNAL(currentIndexChanged(int)), this, SLOT(on_model_currentIndexChanged(int)));
}

void SaveData::on_chose_file_clicked()
{
	QTextCodec *code = QTextCodec::codecForName("GB2312");
	std::string workPath = code->fromUnicode(QFileDialog::getExistingDirectory(this, tr("Open Directory"), " ", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)).data();
	qDebug() << QString::fromLocal8Bit(workPath.c_str());
	ui->m_lineEdit_file->setText(QString::fromLocal8Bit(workPath.c_str()));

	QFileInfo qfi(ui->m_lineEdit_file->text());
	if (qfi.isDir())
	{
		ui->m_lab_display_status->setText("Save Path:" + ui->m_lineEdit_file->text());
	}
	else
	{
		ui->m_lab_display_status->setText("Save Path Error");
	}
}

void SaveData::on_start_clicked()
{
	m_bIsStart = true;//启动保存图像标志
	m_nSaveCount = ui->m_lineEdit_pfs_counts->text().toInt();//保存图像帧数
	QDir qd;
	QString savePath = ui->m_lineEdit_file->text();
	if (savePath.toStdString().empty())
	{
		ui->m_lineEdit_file->setText("请选择完整的保存路径!");
		return;
	}
	if (m_nSaveCount == 0)
	{
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

	switch (m_eCurSaveMode)
	{
	case ESaveMode_3Pix:
	{
		qd.mkdir(savePath);
		qd.mkdir(savePath + "//rgb");
		qd.mkdir(savePath + "//ir");
		qd.mkdir(savePath + "//depth");
		qd.mkdir(savePath + "//rgb-alone");
	}
		break;
	case ESaveMode_4Pix:
	{
		qd.mkdir(savePath);
		qd.mkdir(savePath + "//rgb");
		qd.mkdir(savePath + "//ir");
		qd.mkdir(savePath + "//depth");
		qd.mkdir(savePath + "//pointcloud");
	}
		break;
	default:
		break;
	}

	emit SendSaveDataStatus(m_bIsStart, m_eCurSaveMode, m_nSaveCount, savePath);
}

void SaveData::on_stop_clicked()
{
	m_bIsStart = false;
	emit SendSaveDataStatus(m_bIsStart, m_eCurSaveMode, m_nSaveCount, QString("./data"));
}

void SaveData::on_model_currentIndexChanged(int nIndex)
{
	m_eCurSaveMode = (ESaveMode)nIndex;
}
