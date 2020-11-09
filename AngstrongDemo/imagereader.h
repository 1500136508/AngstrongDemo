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

    void run();
    void run(int camIndex);

private:
    QMutex qMutex;
    std::vector<cv::Mat> container;

    dataSaver *dsaver;

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
    int readIndex = 1;
    bool serverOK = false;
    bool readDataFinish = true;
    bool calcArea = false;
    int save_count = 0;
    int max_count = 0;
    int save_mode = 3;
    bool save_stop = false;
    int mouseX = 0;
    int mouseY = 0;


    cv::Mat edge = cv::Mat::zeros(cv::Size(frameHeight,frameWidth),CV_8UC1);
    cv::Mat edge_clear = cv::Mat::zeros(cv::Size(frameWidth,frameHeight),CV_8UC1);
    cv::Mat edge_th = cv::Mat::zeros(cv::Size(frameWidth,frameHeight),CV_8UC1);

//    std::queue<unsigned char*> imageQueue;
    unsigned char* imageQueue[10];
    int queuefront = -1;
    int queuetail = -1;

    uchar* datagroup;
    uchar* datagroupR;
    uchar* rebulidRgb;
    cv::Mat irFrame;
    cv::Mat irFrameAlign;
    cv::Mat temp;
    cv::Mat depthFrame;
    cv::Mat RGBFrame;
    cv::Mat combineFrame;
    cv::Mat irFrame16bit;
    char frameRateData[30];

    cv::Mat irFrameOri;

    logWriter *lwriter;

    unsigned char* irData;
    unsigned char* irDataGamma;
    float* depthData;
    float* predepthData;
    float* depthDataRGB;
    float* tmpdepth;
    unsigned char* rgbData;


    unsigned char* _buf;
    unsigned char* _buf2;
    void combineData();

    int socketInteract(int flag);

    std::string savePath = "data";

    cv::Mat IRDATABUFFER[10];
    cv::Mat RGBDATABUFFER[10];
    float* depthBuffer[10];
    float* cloudptsBuffer[10];
    float* predepthBuffer[10];
    cv::Mat irFrameOriBuffer[10];
    long long rgbTs[10];
    long long irTs[10];
    long long depthTs[10];
    long long rgbT, irT,depthT;

    void saveDataThread();

    bool isRunning = false;
    bool isGoingToSaveData = false;
    bool quitProgram = false;
    bool initFinish = false;

    bool getFirstArea = false;
    bool getSecondArea = false;

    bool flag_ir = false;
    bool flag_rgb =false;
    bool flag_depth =false;
    bool flag_rd=false;

    int realX1 = -1;
    int realY1 = -1;
    int realX2 = -1;
    int realY2 = -1;

    int realX1s = -1;
    int realY1s = -1;
    int realX2s = -1;
    int realY2s = -1;

    int isWarp = false;

    CCameraDS* camds;

    QThreadPool* poolDepth;

    void checkThread();
    void recvData();

    void readImageThread();
    void buildDataThread();

    QString savepth = "./data";
private slots:

    void saveDataSlot(int flag);
    void saveModeSetSlot(int mode);
    void saveStopSlot();
    void savePathSlot(QString savePath);

    void acceptLocation(int x,int y);
    void acceptArea(int x1,int y1,int x2,int y2,int flag);
    void stopingProgram();
    void readXMLData(QString xmlFile);
    void readpdData();
    void setShowPic(int flag, bool enable);

signals:
    void sendImage(cv::Mat);
    void sendLostServer(bool);
    void sendStopSignal(bool);
    void sendLocationDepth(int x,int y, float depth);
    void sendAreaAvg(float avg0,float avg1);
    void sendSaveDone();
    void sendFrameRate(QString,QString,QString);

    void sendSaveInfo(QString saveInfo);

};

#endif // IMAGEREADER_H
