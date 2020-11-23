#include <QDebug>
#include "displayview.h"
#include "ui_displayview.h"

DisplayView::DisplayView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayView)
{
    ui->setupUi(this);
}

DisplayView::~DisplayView()
{
    delete ui;
}

void DisplayView::ReceiveImageGray(int r, int g, int b)
{
	ui->m_lab_rgb->setText("(R:" + QString::number(r) + " G:" + QString::number(g)+" B:" + QString::number(b)+")");
}

void DisplayView::ReceiveAvgDepth(float avg0, float avg1)
{
	//QString qstrAvgInfo("");
	ui->m_lab_avg_depth01->setText(QString::number(avg0));
	ui->m_lab_avg_depth02->setText(QString::number(avg1));
}

void DisplayView::ReceiveLocationDepth(int x, int y, float depth)
{
	QString info;
	info.sprintf("%.3f", depth);
	qDebug() << depth;
	ui->m_lab_mouse_xy->setText("X:" + QString::number(x) + " Y:" + QString::number(y));
	ui->m_lab_depth->setText(info);
}