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
#include "ui_imageview.h"
#include "imagereader.h"

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
signals:
	void sendCloseEven();
public:
    Ui::ImageView *ui;

	//����ɼ�ͼ�����
	imageReader *m_pCamera;
	QString m_qstrSavePath;
	bool m_bIsSaveRGB;
	bool m_bIsSaveDepth;
	bool m_bIsSaveIR;
	bool m_bIsSavePointCloud;
private:
	void BuildConnet();

	QString stylesheet;//qss��������

	cv::Mat			m_cvImageRGB[BUFFSIZE];//����RGBͼ������
	cv::Mat			m_cvImageIR[BUFFSIZE];//����IRͼ������
	float			*m_pDataDepth[BUFFSIZE];//����Depth����
	int             m_nSaveDataIndex;//ͼ�����黺�����洢���,ÿ����Ŷ�Ӧһ��ͼƬ
};

#endif // IMAGEVIEW_H
