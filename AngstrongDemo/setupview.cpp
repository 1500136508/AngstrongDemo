#include "setupview.h"
#include "ui_setupview.h"
#include <QFile>
#include <QTextStream>

SetupView::SetupView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupView)
{
    ui->setupUi(this);
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
}

void SetupView::InitParameter()
{
	ConfigInstace data_manager("parameter.ini");
	std::string image_view_count = data_manager.GetString("[ImageView]", "ImageView_Count");
	ui->m_lineEdit_imageview_count->setText(QString::fromStdString(image_view_count));
}

void SetupView::on_save_parameter_clicked()
{
	ConfigInstace data_manager("parameter.ini");
	QString image_view_count = ui->m_lineEdit_imageview_count->text();
	data_manager.SetString("[ImageView]", "ImageView_Count", image_view_count.toStdString());
}