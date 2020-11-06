#include "outputview.h"
#include "ui_outputview.h"

OutputView::OutputView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputView)
{
    ui->setupUi(this);
}

OutputView::~OutputView()
{
    delete ui;
}
