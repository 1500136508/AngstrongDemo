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

	//����ɼ�ͼ�����
	imageReader *m_pCamera;
	QString m_qstrSavePath;
	bool m_bIsSaveRGB;
	bool m_bIsSaveDepth;
	bool m_bIsSaveIR;
	bool m_bIsSavePointCloud;
private:
	void BuildConnet();
	void SaveImageThread();

	QString stylesheet;//qss��������

	cv::Mat			m_cvImageRGB[BUFFSIZE];//����RGBͼ������
	cv::Mat			m_cvImageIR[BUFFSIZE];//����IRͼ������
	float			*m_pDataDepth[BUFFSIZE];//����Depth����
	int             m_nSaveDataIndex;//ͼ�����黺�����洢���,ÿ����Ŷ�Ӧһ��ͼƬ
	int             m_nSaveImageCount;//����ͼ������
	int				m_nMode;//����ͼ���ģʽ
	int             m_nWriteIndex;//���ڱ���ͼ������
	bool            m_bSaveFinish;//������ɱ�־
	volatile bool   m_bIsSaveImage;//�Ƿ񱣴�ͼƬ

	//�̹߳���
	std::thread *m_pThreadSaveImage;
	std::mutex m_mutex;
};

#endif // IMAGEVIEW_H
