#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QDialog>
#include <core.hpp>
#include "ui_imageview.h"

namespace Ui {
class ImageView;
}

class ImageView : public QDialog
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

public:
    Ui::ImageView *ui;
	cv::Mat m;
};

#endif // IMAGEVIEW_H
