//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to:
//		HardyAI@OpenCV China
//		flymanbox@OpenCV China (for his contribution to function CameraName, and frame width/height setting)
// Last modification: April 9, 2009
//
// 使用说明：
//   1. 将CameraDS.h CameraDS.cpp以及目录DirectShow复制到你的项目中
//   2. 菜单 Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      设置为 DirectShow/Include
//   3. 菜单 Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      设置为 DirectShow/Lib
//////////////////////////////////////////////////////////////////////
#define DS_CAP_PROP_EXPOSURE  1
#define DS_CAP_PROP_BRIGHTNESS  2
#define DS_CAP_PROP_CONTRAST  3
#define DS_CAP_PROP_HUE  4
#define DS_CAP_PROP_SATURATION  5
#define DS_CAP_PROP_SHARPNESS  6
#define DS_CAP_PROP_GAMMA  7
#define DS_CAP_PROP_GAIN 8

#ifndef CCAMERA_H
#define CCAMERA_H

#define WIN32_LEAN_AND_MEAN

#include <atlbase.h>

#include "qedit.h"
#include "dshow.h"

#include <windows.h>
//#include <opencv/cxcore.h>	// lyd
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


class  CCameraDS
{
private:

    bool m_bConnected, m_bLock, m_bChanged;

    int m_nWidth, m_nHeight;
    int m_nFormatType;

    long m_nBufferSize;
    unsigned short *m_nBuffer;
    IplImage *m_pFrame;
    CComPtr<IAMCameraControl> m_pCameraControl;
    CComPtr<IAMVideoProcAmp> m_pVideoProAmp;
    CComPtr<IGraphBuilder> m_pGraph;
    CComPtr<ISampleGrabber> m_pSampleGrabber;
    CComPtr<IMediaControl> m_pMediaControl;
    CComPtr<IMediaEvent> m_pMediaEvent;
    CComPtr<IBaseFilter> m_pSampleGrabberFilter;
    CComPtr<IBaseFilter> m_pDeviceFilter;
    CComPtr<IBaseFilter> m_pNullFilter;

    CComPtr<IPin> m_pGrabberInput;
    CComPtr<IPin> m_pGrabberOutput;
    CComPtr<IPin> m_pCameraOutput;
    CComPtr<IPin> m_pNullInputPin;

    bool BindFilter(int nCamIDX, IBaseFilter **pFilter);
    bool BindFilter(IBaseFilter **pFilter,std::string pid,std::string vid);

    void SetCrossBar();

    bool isFormatYUY2 = false;

public:

    CCameraDS();
    virtual ~CCameraDS();

    //打开摄像头，nCamID指定打开哪个摄像头，取值可以为0,1,2,...
    //bDisplayProperties指示是否自动弹出摄像头属性页
    //nWidth和nHeight设置的摄像头的宽和高，如果摄像头不支持所设定的宽度和高度，则返回false
    bool OpenCamera(int nCamID, int nWidth = 1280, int nHeight = 800, bool isYUV2 = true);
    bool OpenCamera(std::string pid, std::string vid, int nWidth= 1280, int nHeight=800, bool isYUV2 = true);
    //关闭摄像头，析构函数会自动调用这个函数
    void CloseCamera();

    bool isOpened();
    //返回摄像头的数目
    //可以不用创建CCameraDS实例，采用int c=CCameraDS::CameraCount();得到结果。
    static int CameraCount();

    //根据摄像头的编号返回摄像头的名字
    //nCamID: 摄像头编号
    //sName: 用于存放摄像头名字的数组
    //nBufferSize: sName的大小
    //可以不用创建CCameraDS实例，采用CCameraDS::CameraName();得到结果。
    static int CameraName(int nCamID, char* sName, int nBufferSize);

    //返回图像宽度
    int GetWidth(){return m_nWidth;}

    //返回图像高度
    int GetHeight(){return m_nHeight;}

    //抓取一帧，返回的IplImage不可手动释放！
    //返回图像数据的为RGB模式的Top-down(第一个字节为左上角像素)，即IplImage::origin=0(IPL_ORIGIN_TL)
    IplImage * QueryFrame();

    bool read(cv::Mat &outputArray);
    bool readRawData(unsigned char* data);

    bool SetExposure(int iExposure);
    bool getID(int &id);
    bool SetAutoExposure();
    bool Set(int flag, int value);
};

#endif
