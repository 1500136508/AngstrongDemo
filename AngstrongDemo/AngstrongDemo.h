#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include "ui_AngstrongDemo.h"
#include "imageview.h"
#include "cameraview.h"
#include "parameterview.h"
#include "outputview.h"

class AngstrongDemo : public QMainWindow
{
    Q_OBJECT

public:
    AngstrongDemo(QWidget *parent = Q_NULLPTR);
	virtual ~AngstrongDemo();
private slots:
	void on_ShowDlg_clicked();
private:
	void CreateDockWindow();
private:
    Ui::AngstrongDemoClass ui;

	ImageView m_ImageView;
	CameraView m_CameraView;
	ParameterView m_ParamView;
	OutputView m_OutputView;
};
