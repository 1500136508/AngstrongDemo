#include "outputview.h"
#include "ui_outputview.h"
#include "logmanager.h"

OutputView *OutputView::output_view_ = nullptr;
std::mutex OutputView::mutex_;
OutputView::OutputView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputView)
{
    ui->setupUi(this);
}

OutputView::~OutputView()
{
	delete ui;
	if (output_view_)
	{
		output_view_->deleteLater();
		output_view_ = nullptr;
	}
}

OutputView *OutputView::GetInstance()
{
	std::unique_lock<std::mutex> locker(mutex_);
	if (!output_view_)
	{
		output_view_ = new OutputView();
	}
	locker.unlock();
	return output_view_;
}
void OutputView::WriteText(QString output_msg)
{
	output_view_->ui->textBrowser->append(output_msg);
}