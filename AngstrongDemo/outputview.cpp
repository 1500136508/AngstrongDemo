#include "outputview.h"
#include "ui_outputview.h"
#include "logmanager.h"

OutputView *OutputView::output_view_ = nullptr;
OutputView::OutputView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputView)
{
    ui->setupUi(this);
}

OutputView::OutputView(const OutputView &)
{
	int a = 0;
}

OutputView & OutputView::operator=(const OutputView &)
{
	// TODO: �ڴ˴����� return ���
	return *output_view_;
}

OutputView::~OutputView()
{
	delete ui;
	if (output_view_)
	{
		delete output_view_;
		output_view_ = nullptr;
	}
}

OutputView *OutputView::GetInstance()
{
	if (!output_view_)
	{
		output_view_ = new OutputView();
	}
	return output_view_;
}
void OutputView::WriteText(QString output_msg)
{
	output_view_->ui->textBrowser->append(output_msg);
}
