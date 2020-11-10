#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QDialog>
#include <core.hpp>
#include <memory>
#include <functional>
#include <vector>
#include <QString>
#include <QMenu>
#include <QAction>
#include "ui_imageview.h"
#include "imagescene.h"
#include "imageitem.h"
#include "imagepix.h"
#include "definitionmenu.h"

namespace Ui {
class ImageView;
}

class ImageView : public QDialog
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

	void ZoomFit();
protected:
	bool Open();//Only Framework

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *);
private slots:
	void on_open_clicked();
	void on_save_clicked();
	void on_close_clicked();
	void on_zoomIn_clicked();
	void on_zoomOut_clicked();
	void on_zoomFit_clicked();
	void on_measure_clicked();
	void contextMenuRequest(QPoint);
public:
    Ui::ImageView *ui;
	cv::Mat m_Image;
private:
	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);

	std::shared_ptr<ImageScene> m_spScene;
	std::shared_ptr<ImageItem>  m_spItem;
	std::shared_ptr<ImagePix>   m_spPix;
private:
};

#endif // IMAGEVIEW_H
