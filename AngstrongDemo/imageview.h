#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QDialog>
#include <core.hpp>
#include <highgui.hpp>
#include <opencv.hpp>
#include <memory>
#include <functional>
#include <vector>
#include <QString>
#include <QMenu>
#include <QAction>
#include <thread>
#include <mutex>
#include "ui_imageview.h"
#include "imagereader.h"
#include "parameterview.h"

#define BUFFSIZE 1000

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
private slots:
	void ReceiveImageInfo(bool bIsEmpty, int nImageWidth, int nImageHeight);
	void ReceiveMouseInfo(int x, int y);
	void ReceiveImageGray(int R,int G,int B);
	void ReceiveSaveImageData(cv::Mat ImageIR, cv::Mat ImageRGB, float *depth);
	void ReceiveSaveDataStatus(bool bSave, int eMode, int nSaveCoun,QString qstrPath);

	void ReceiveCameraStatus(ECameraStatus eStatus);
signals:
	void sendCloseEven();
	void SendCameraStatus(ECameraStatus eStatus = ECameraStatus_Unknow);
	void SendSaveImageInfo(QString qstrInfo);
public:
    Ui::ImageView *ui;
	bool m_bProgramExit;

	//相机采集图像相关
	imageReader *m_pCamera;
	QString m_qstrSavePath;
	bool m_bIsSaveRGB;
	bool m_bIsSaveDepth;
	bool m_bIsSaveIR;
	bool m_bIsSavePointCloud;
private:
	void BuildConnet();
	void SaveImageThread();

	QString stylesheet;//qss界面美化

	cv::Mat			m_cvImageRGB[BUFFSIZE];//接收RGB图像数组
	cv::Mat			m_cvImageIR[BUFFSIZE];//接收IR图像数组
	float			*m_pDataDepth[BUFFSIZE];//接收Depth数据
	int             m_nSaveDataIndex;//图像数组缓冲区存储序号,每个序号对应一张图片
	int             m_nSaveImageCount;//保存图像数量
	int				m_nMode;//保存图像的模式
	int             m_nWriteIndex;//正在保存图像的序号
	bool            m_bSaveFinish;//保存完成标志
	volatile bool   m_bIsSaveImage;//是否保存图片

	//线程管理
	std::thread *m_pThreadSaveImage;
	std::mutex m_mutex;
};

#endif // IMAGEVIEW_H
