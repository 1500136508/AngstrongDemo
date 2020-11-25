#ifndef IMAGEREADER_H
#define IMAGEREADER_H
#include <opencv.hpp>
#include <QThread>
#include <QString>
#include "camerads.h"
#include <QThreadPool>
#include <queue>
#include <QMutex>
#include <fstream>
#include <QRectF>
//#include "logwriter.h"
#define EFE_FORMAT
//#define KEEP_ORI

class imageReader:public QObject
{
    Q_OBJECT
public:
    imageReader(QObject *parent=NULL);
    ~imageReader();

    void release();
	void run(int camIndex);
	bool IsRunning()const;
	bool OpenCamera(int index);
	void CloseCamera();
	bool IsOpen()const;
	void Live();
	void Pause();
	void Stop();
	void SetSaveImageStatus(bool bIsSaveImage);

	float fx = 0;
	float fy = 0;
	float cx = 0;
	float cy = 0;
private:
    std::vector<cv::Mat> container;

    clock_t time1,time2,startTime, stopTime;
    int datalen = 1280;

    int frameHeight = 400;
    int frameWidth = 640;
#ifdef EFE_FORMAT
    int frameHeightR = 480;
    int frameWidthR = 848;
#else
    int frameHeightR = 400;
    int frameWidthR = 640;
#endif

    cv::Mat edge = cv::Mat::zeros(cv::Size(frameHeight,frameWidth),CV_8UC1);
    cv::Mat edge_clear = cv::Mat::zeros(cv::Size(frameWidth,frameHeight),CV_8UC1);
    cv::Mat edge_th = cv::Mat::zeros(cv::Size(frameWidth,frameHeight),CV_8UC1);

    uchar* datagroup;
    uchar* datagroupR;
    cv::Mat irFrame;
    cv::Mat irFrameAlign;
    cv::Mat temp;
    cv::Mat depthFrame;
    cv::Mat RGBFrame;
    cv::Mat combineFrame;
    cv::Mat irFrame16bit;

    unsigned char* irData;
    unsigned char* irDataGamma;
    float* depthData;
    float* predepthData;
    float* depthDataRGB;
    float* tmpdepth;
    unsigned char* rgbData;


    unsigned char* _buf;
    unsigned char* _buf2;


    long long rgbT, irT,depthT;


    bool isRunning = false;
    bool quitProgram = false;


    bool flag_ir = false;
    bool flag_rgb =false;
    bool flag_depth =false;
    bool flag_rd=false;

    CCameraDS* camds;

    QThreadPool* poolDepth;

	//当前鼠标位置
	int m_MouseX;
	int m_MouseY;

	//AvgArea
	bool calcArea = false;
	bool getFirstArea = false;
	bool getSecondArea = false;
	int isWarp = false;

	int realX1 = -1;
	int realY1 = -1;
	int realX2 = -1;
	int realY2 = -1;

	int realX1s = -1;
	int realY1s = -1;
	int realX2s = -1;
	int realY2s = -1;

    void buildDataThread();
	// 设置内外参
	int setParam(float _fx, float _fy, float _cx, float _cy);
	volatile bool m_bIsSaveImage;
private slots:
    void readpdData();
	void ReceiveMouseInfo(int x, int y);
	void ReceiveAvgArea(int nIndxe, QRectF rect);
signals:
    void sendImage(cv::Mat);
	void sendSaveImageData(cv::Mat ImageIR, cv::Mat ImageRGB,float *depth);
	void SendLocationDepth(int x, int y, float depth);
	void SendAvgDepth(float avg0, float avg1);
};

#endif // IMAGEREADER_H
