#ifndef IMAGEREADER_H
#define IMAGEREADER_H
#include <opencv.hpp>
#include <QThread>
#include <QString>
#include "camerads.h"
#include <QThreadPool>
#include <queue>
#include <QMutex>
#include "datasaver.h"
#include <fstream>
#include "logwriter.h"
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
	void OpenCamera(int index);
	void CloseCamera();
	void Live();
	void Pause();
	void Stop();
	void SetSaveImageStatus(bool bIsSaveImage);

	dataSaver *dsaver;
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

    void buildDataThread();

	volatile bool m_bIsSaveImage;
private slots:
    void readpdData();
signals:
    void sendImage(cv::Mat);
	void sendSaveImageData(cv::Mat ImageIR, cv::Mat ImageRGB,float *depth);
};

#endif // IMAGEREADER_H
