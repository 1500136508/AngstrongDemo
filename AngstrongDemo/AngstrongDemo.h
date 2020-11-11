#pragma once

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include <thread>
#include <vector>
#include <memory>
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
private:
	void CreateDockWindow();
	void AddToolBar();
	void BuildConnect();
private:
    Ui::AngstrongDemoClass ui;

	std::vector<std::shared_ptr<ImageView>> m_vec_spImageView;
	CameraView m_CameraView;
	ParameterView m_ParamView;
	OutputView m_OutputView;

	std::thread m_thread_run;
};
