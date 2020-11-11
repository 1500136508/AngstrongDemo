#include "datasaver.h"
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <Windows.h>
#define BUFFLEN 1000

dataSaver::dataSaver()
{
    for (int j = 0 ; j < BUFFLEN;j++){
        depthData[j] = new float[848*480];
    }
    aloneTimeStamp = new long long [BUFFLEN];

    QtConcurrent::run(this,&dataSaver::saveDataToDevice);
}

dataSaver::~dataSaver()
{
    for (int j = 0 ; j < BUFFLEN;j++){
        delete[] depthData[j] ;
    }
    delete [] aloneTimeStamp;
    quitProgramm = true;
}

int dataSaver::setSavePathAndMode(QString _savePath,int mode)
{
    savePath = std::string(_savePath.toLocal8Bit());
    printf("%s\n",savePath.c_str());
    if (mode!=3 && mode!=4) return -1;
    else saveMode = mode;

    return 0;
}


int dataSaver::storeData(cv::Mat irF, cv::Mat rgbF, float *depth)
{
    irF.copyTo(irFrameData[saveIndex]);
    rgbF.copyTo(rgbFrameData[saveIndex]);
#ifndef KEEP_ORI
    memcpy(depthData[saveIndex],depth,848*480*sizeof(float));
#else
    memcpy(depthData[saveIndex],depth,640*400*sizeof(float));
#endif
    saveIndex += 1;
    if(saveIndex == BUFFLEN){
        saveIndex = 0;
    }
    return 0;
}

int dataSaver::storeAloneRGBData(cv::Mat rgbFA, long long timeStamp)
{
    rgbFA.copyTo(rgbAloneFrameData[aloneSaveIndex]);
    aloneTimeStamp[aloneSaveIndex] = timeStamp;
    aloneSaveIndex += 1;
    if(aloneSaveIndex == BUFFLEN){
        aloneSaveIndex = 0;
    }
    return  0;
}

int dataSaver::setParam(float _fx, float _fy, float _cx, float _cy)
{
	fx = _fx;
	fy = _fy;
	cx = _cx;
	cy = _cy;
	return  0;
}

int dataSaver::checkReady()
{
    return writeFinsih;
}

void dataSaver::releaseDateSaver()
{
    quitProgramm = true;
}

int dataSaver::saveDataToDevice()
{
    int frameHeightR = 480;
    int frameWidthR = 848;

    int frameHeight = 400;
    int frameWidth = 640;
    FILE *plyfile;
    int stopCheck = 0;
    while (!quitProgramm){

        if (writeIndex != saveIndex){
            stopCheck = 0;
            writeFinsih = false;
            if (saveMode == 3){
                std::string fileName = std::to_string(savePathIndex);
                int zeroNumber = 0;
                int tmp = savePathIndex;
                while (tmp > 0){
                    zeroNumber += 1;
                    tmp /= 10;
                }
                for (int k =0;k<4-zeroNumber;k++)  fileName = "0"+fileName;
                std::string Pth = savePath+"//ir//"+fileName+".png";

                cv::cvtColor(irFrameData[writeIndex],irFrameData[writeIndex],cv::COLOR_RGB2GRAY);
                cv::imwrite(Pth,irFrameData[writeIndex]);

                Pth = savePath+"//depth//"+fileName+".png";
#ifndef KEEP_ORI
                cv::Mat saveMat = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_16UC1);
                for (int x = 0; x < frameHeightR; x++) {
                    for (int y = 0; y < frameWidthR; y++) {
                        saveMat.at<short>(y, x) = short(depthData[writeIndex][y*frameHeightR + x]);
                    }
                }
#else
                cv::Mat saveMat = cv::Mat(cv::Size(frameHeight,frameWidth),CV_16UC1);
                for (int x = 0; x < frameHeight; x++) {
                    for (int y = 0; y < frameWidth; y++) {
                        saveMat.at<short>(y, x) = short(depthData[writeIndex][y*frameHeight + x]);
                    }
                }
#endif
                cv::imwrite(Pth,saveMat);
                Pth = savePath+"//rgb//"+fileName+".jpg";
                cv::imwrite(Pth,rgbFrameData[writeIndex]);

            }
            else if (saveMode == 4){

                std::string fileName = std::to_string(savePathIndex);
                int zeroNumber = 0;
                int tmp = savePathIndex;
                while (tmp > 0){
                    zeroNumber += 1;
                    tmp /= 10;
                }
                for (int k =0;k<4-zeroNumber;k++)  fileName = "0"+fileName;
                std::string Pth = savePath+"//ir//"+fileName+".png";
                cv::cvtColor(irFrameData[writeIndex],irFrameData[writeIndex],cv::COLOR_RGB2GRAY);
                cv::imwrite(Pth,irFrameData[writeIndex]);

                Pth = savePath+"//depth//"+fileName+".png";
//                cv::Mat saveMat = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_16UC1);
//                for (int x = 0; x < frameHeightR; x++) {
//                    for (int y = 0; y < frameWidthR; y++) {
//                        saveMat.at<short>(y, x) = short(depthData[writeIndex][y*frameHeightR + x]);
//                    }
//                }
//                cv::imwrite(Pth,saveMat);

#ifndef KEEP_ORI
                cv::Mat saveMat = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_16UC1);
                for (int x = 0; x < frameHeightR; x++) {
                    for (int y = 0; y < frameWidthR; y++) {
                        saveMat.at<short>(y, x) = short(depthData[writeIndex][y*frameHeightR + x]);
                    }
                }
#else
                cv::Mat saveMat = cv::Mat(cv::Size(frameHeight,frameWidth),CV_16UC1);
                for (int x = 0; x < frameHeight; x++) {
                    for (int y = 0; y < frameWidth; y++) {
                        saveMat.at<short>(y, x) = short(depthData[writeIndex][y*frameHeight + x]);
                    }
                }
#endif
                cv::imwrite(Pth,saveMat);


                Pth = savePath+"//pointcloud//"+fileName+".txt";
                fopen_s(&plyfile, Pth.c_str(), "w");
                for (int x = 0; x < frameHeightR; x++) {
                    for (int y = 0; y < frameWidthR; y++) {
                        float z = depthData[writeIndex][y*frameHeightR + x];
                        if (z < 10e-6 && z > -10e-6) continue;
                        float x_rw = ((float)x - cx) * z / fx;
                        float y_rw = ((float)y - cy) * z / fy;
                        fprintf(plyfile, "%.6f %.6f %.6f\n", x_rw, y_rw, z);
                    }
                }
                fclose(plyfile);
                Pth = savePath+"//rgb//"+fileName+".jpg";
                cv::imwrite(Pth,rgbFrameData[writeIndex]);
            }
            savePathIndex += 1;
            writeIndex += 1;
            if (writeIndex == BUFFLEN) writeIndex = 0;
        }
        if (aloneWriteIndex != aloneSaveIndex){
            writeFinsih = false;
            std::string Pth = savePath+"//rgb-alone//"+std::to_string(aloneTimeStamp[aloneWriteIndex])+".jpg";
            qDebug()<<aloneTimeStamp[aloneWriteIndex];
            cv::imwrite(Pth,rgbAloneFrameData[aloneWriteIndex]);
            aloneWriteIndex+=1;
            if (aloneWriteIndex == BUFFLEN) aloneWriteIndex += 1;
        }
        if (writeIndex == saveIndex && aloneWriteIndex == aloneSaveIndex && writeFinsih == false)
        {
            qDebug()<<"stopCheck"<<stopCheck;
            if (stopCheck==5){
                savePathIndex = 0;
                stopCheck = 0;
                writeFinsih = true;
            }
            else{
                stopCheck += 1;
                Sleep(100);
            }
        }
    }
    return 0;
}
