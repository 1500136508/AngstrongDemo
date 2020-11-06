#include "parameterview.h"
#include "ui_parameterview.h"

ParameterView::ParameterView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterView)
{
    ui->setupUi(this);
}

ParameterView::~ParameterView()
{
    delete ui;
}
