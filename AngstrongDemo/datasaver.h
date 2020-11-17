#ifndef DATASAVER_H
#define DATASAVER_H

#include <QObject>
#include <opencv.hpp>
#include <string>

//#define KEEP_ORI
class dataSaver
{
public:
    dataSaver();
    ~dataSaver();

    // 设置保存路径及模式
    int setSavePathAndMode(QString _savePath,int mode);
    // 传入保存数据
    int storeData(cv::Mat irF, cv::Mat rgbF, float* depth);
    // 传入单张保存的RGB数据
    int storeAloneRGBData(cv::Mat rgbFA, long long timeStamp);
    // 设置内外参
    int setParam(float _fx,float _fy, float _cx,float _cy);
    // 检测是否可以开始保存
    int checkReady();
    void releaseDateSaver();
	float fx = 0;
	float fy = 0;
	float cx = 0;
	float cy = 0;
private:
    std::string savePath = ".";
   /* float fx = 0;
    float fy = 0;
    float cx = 0;
    float cy = 0;*/
    int saveMode = 3;
    cv::Mat irFrameData[1000];
    cv::Mat rgbFrameData[1000];
    cv::Mat rgbAloneFrameData[1000];
    float* depthData[1000];
    long long * aloneTimeStamp;

    bool firstFull = false;
    bool secondFull = false;
    bool thirdFull = false;

    int saveIndex = 0;
    int aloneSaveIndex = 0;
    int writeIndex = 0;
    int aloneWriteIndex = 0;
    int saveDataToDevice();


    int savePathIndex = 0;
    cv::Mat lastRGB;

    bool writeFinsih = true;
	bool quitProgramm = false;

    bool readyToSave = false;
};

#endif // DATASAVER_H
