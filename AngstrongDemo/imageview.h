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
#include "imagereader.h"

namespace Ui {
class ImageView;
}

class ImageView : public QDialog
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *);
	bool eventFilter(QObject *obj, QEvent *event);
	void closeEvent(QCloseEvent *event);

signals:
	void sendCloseEven();
public:
    Ui::ImageView *ui;

	//相机采集图像相关类
	imageReader *m_pCamera;
private:
	void BuildConnet();

	bool cameraReady = false;

	QString stylesheet;//qss界面美化
};

#endif // IMAGEVIEW_H
