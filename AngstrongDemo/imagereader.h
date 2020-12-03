#ifndef IMAGEREADER_H
#define IMAGEREADER_H
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <QObject>
#include <QRectF>
#include <QString>
#include <opencv.hpp>
#include "camerads.h"
#define EFE_FORMAT
//#define KEEP_ORI

class imageReader:public QObject, public ISampleGrabberCB
{
	enum EIMAGETYPE
	{
		EIMAGETYPE_RGB = 0,
		EIMAGETYPE_IR,
		EIMAGETYPE_DEPTH,
	};
    Q_OBJECT
public:
    imageReader(QObject *parent=NULL);
    ~imageReader();

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

	HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
	HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, long BufferLen);

	bool OpenCamera(int camera_index);
	void CloseCamera();
	bool IsOpen()const;
	bool IsRunning()const;
	void Live();
	void Pause();
	void Stop();
	void SetSaveImageStatus(bool bIsSaveImage);
	void release();

	float fx = 0;
	float fy = 0;
	float cx = 0;
	float cy = 0;
private:
	std::thread thread_get_rgb_image_;
	std::thread thread_get_depth_image_;
	std::thread thread_get_ir_image_;
	std::thread thread_wait_grab_image_finished_;
	std::mutex mutex_;
	volatile bool get_rgb_image_finished_ = false;
	volatile bool get_depth_image_finished_ = false;
	volatile bool get_ir_image_finished_ = false;
	volatile bool grab_image_finished_ = false;
	volatile bool is_first_time_grab_ = true;
    std::vector<cv::Mat> container;

    clock_t time1,time2,startTime, stopTime;
    int datalen = 1280;

    const int frameHeight = 400;
    const int frameWidth = 640;
#ifdef EFE_FORMAT
    const int frameHeightRGB = 480;
    const int frameWidthRGB = 848;
#else
    const int frameHeightRGB = 400;
    const int frameWidthRGB = 640;
#endif

    cv::Mat edge = cv::Mat::zeros(cv::Size(frameHeight,frameWidth),CV_8UC1);
    cv::Mat edge_clear = cv::Mat::zeros(cv::Size(frameWidth,frameHeight),CV_8UC1);
    cv::Mat edge_th = cv::Mat::zeros(cv::Size(frameWidth,frameHeight),CV_8UC1);

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


	long long rgbT;
	long long irT;
	long long depthT;
	long long lastRgbT;

	bool getParam = false;
	volatile bool is_running_ = false;

    bool flag_ir = false;
    bool flag_rgb =false;
    bool flag_depth =false;
    bool flag_rd=false;

    CCameraDS* camds;
	
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

	void GetRGBImageThread(BYTE *rgb_image_data);
	void GetDepthImageThread(BYTE *depth_image_data);
	void GetIRImageThread(BYTE *ir_image_data);
    void WaitGrabImageFinished();
	void RunThread(BYTE *image_data);

	void GetRGBImage(BYTE *rgb_image_data);
	void GetDepthImage(BYTE *depth_image_data);
	void GetIRImage(BYTE *ir_image_data);

	bool IsNewImageData(uchar *image_data);
	void InitPDData(uchar *image_data);
	bool IsInitPDData()const { return getParam; }
	void DispImage();
	void SendDepthImageData(float *depth_image_data);
	void CalcAvgDepthData(float *depth_image_data);

	void WriteImageBinFile(uchar *image_data,long long size);
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
