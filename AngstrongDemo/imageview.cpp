#include "imageview.h"

ImageView::ImageView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);

	//setWindowFlags(Qt::FramelessWindowHint);
}

ImageView::~ImageView()
{
    delete ui;
}
