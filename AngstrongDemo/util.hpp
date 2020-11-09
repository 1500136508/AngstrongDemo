#ifndef UTIL_HPP
#define UTIL_HPP
#include <QString>
#include <QByteArray>
#include <opencv.hpp>

QString num2str(int i){
    QString str = QString::number(i);
    if (str.length()>3) str = "";
    else if (str.length() < 3){
        int cyctime = 3 - str.length();
        for (int l = 0; l < cyctime;l++)  str = "0" + str;
    }
    return str;
}

QString num2strhex(int i){
    QString str = QString(QByteArray::number(i, 16));
    if (str.length()>3) str = "";
    else if (str.length() < 3){
        int cyctime = 3 - str.length();
        for (int l = 0; l < cyctime;l++)  str = "0" + str;
    }
    return str;
}


int fun(float x) {
    int y = 0;
    if (200 <= x && x < 400)
        y = 0.375 * x - 75;
    else if (400 <= x && x < 600)
        y = 0.225 *x - 15;
    else if (600 <= x && x < 800)
        y = 0.15 *x + 30;
    else if (800 <= x && x < 2750)
        y = 3.59e-02 *x + 1.21e+02;
    else if (5000 >= x && x >= 2750)
        y = 1.56e-02 * x + 1.77e+02;
    else if (x > 5000)
        y = 255.0;
    return y;
}

int fun2(float x) {
    double y = 0;
    if (200 <= x && x < 800)
        y = 0.375 * x - 75;
    else if (800 <= x && x < 5000)
        y = 0.0071428 * x + 219;
    else if (x >= 5000)
        y = 255;
    return (int)y;
}

cv::Mat dealDepthMap(float* depthMap, int cols, int rows) {
    //float* tmp = new float[cols * rows];
    cv::Mat depth = cv::Mat(rows, cols, CV_32FC1);
    for (int y = 0; y < rows; y++) {
        float* data = depth.ptr<float>(y);
        for (int x = 0; x < cols; x++) {
            data[x] = depthMap[x + y*cols];
        }
    }
    //depth.convertTo(depth2, CV_8UC1);
    //normalize(depth, depth, 0, 1, CV_MINMAX);
    // depth2.copyTo(this->depthMat);
    cv::Mat im_color(rows, cols, CV_32FC3);
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            im_color.at<cv::Vec3f>(j, i)[0] = 60;
            im_color.at<cv::Vec3f>(j, i)[1] = 60;
            if (depthMap[i + j*cols] >= 5000.0 || depthMap[i + j*cols] == 0.0) {
                im_color.at<cv::Vec3f>(j, i)[2] = 0;
            }
            else
                im_color.at<cv::Vec3f>(j, i)[2] = 255 - fun(depth.at<float>(j, i));
        }
    }
    cvtColor(im_color, im_color, CV_HSV2BGR);
    im_color.convertTo(im_color, CV_8UC3);
    //cv::applyColorMap(depth2, im_color,12);

    depth.release();
    return im_color;
}

cv::Mat dealDepthMapColor(float* depthMap, int cols, int rows) {
    //float* tmp = new float[cols * rows];
    cv::Mat depth = cv::Mat(rows, cols, CV_32FC1);
    for (int y = 0; y < rows; y++) {
        float* data = depth.ptr<float>(y);
        for (int x = 0; x < cols; x++) {
            float d = depthMap[x + y*cols];
            if (d >= 5000.0 || d == 0.0) {
                data[x] = 0;
            }
            else {
                data[x] = 255 - fun2(d);  //255
            }
        }
    }
    cv::Mat depth2,im_color;
    depth.convertTo(depth2, CV_8UC1);
    //normalize(depth, depth, 0, 1, CV_MINMAX);
    // depth2.copyTo(this->depthMat);
    im_color.convertTo(im_color, CV_8UC3);
    cv::applyColorMap(depth2, im_color, cv::COLORMAP_JET);

    depth.release();
    return im_color;
}

void moveMat(cv::Mat & src, cv::Mat & _dst, cv::Mat & temp, int targetW, int targetH, int x, int y) {
    cv::Mat dst = cv::Mat::zeros(targetH, targetW, CV_8UC3);
    temp = cv::Mat::zeros(targetH, targetW, CV_8UC3);
    int x1, x2, y1, y2;
    int X1, X2, Y1, Y2;
    int w = src.cols;
    int h = src.rows;

    if (x > 0) {
        x1 = x; x2 = w;
        X1 = 0; X2 = w - x;
    }
    else {
        x1 = 0; x2 = w + x;
        X1 = -x; X2 = w;
    }
    if (y > 0) {
        y1 = y; y2 = h;
        Y1 = 0; Y2 = h - y;
    }
    else {
        y1 = 0; y2 = h + y;
        Y1 = -y; Y2 = h;
    }
    src.colRange(X1, X2).copyTo(dst.colRange(x1, x2));
    dst.rowRange(Y1, Y2).copyTo(temp.rowRange(0, Y2 - Y1));
    dst = cv::Mat::zeros(targetH, targetW, CV_8UC3);
    temp.rowRange(0, Y2 - Y1).copyTo(dst.rowRange(y1, y2));
    dst.copyTo(_dst);
}

void moveMat2(cv::Mat & src, cv::Mat & _dst, cv::Mat & temp, int targetW, int targetH, int x, int y) {
    cv::Mat dst = cv::Mat::zeros(targetH, targetW, CV_8UC3);
    temp = cv::Mat::zeros(targetH, targetW, CV_8UC3);
    int w = src.cols;
    int h = src.rows;

    int copyw =x>=0?MIN(targetW-x,w):w+x;
    int copyh =y>=0?MIN(targetH-y,h):h+y;

    int startX = x>=0? x:0;
    int startY = y>=0? y:0;

    int startx = x>=0?0:-x;
    int starty = y>=0?0:-y;

    cv::Rect roi_rect(startX,startY,copyw,copyh);
    src(cv::Rect(startx,starty,copyw,copyh)).copyTo(dst(roi_rect));
    dst.copyTo(_dst);
}

void moveMat16Bit(cv::Mat & src, cv::Mat & _dst, cv::Mat & temp, int targetW, int targetH, int x, int y) {
    cv::Mat dst = cv::Mat::zeros(targetH, targetW, CV_16UC1);
    temp = cv::Mat::zeros(targetH, targetW, CV_16UC1);
    int x1, x2, y1, y2;
    int X1, X2, Y1, Y2;
    int w = src.cols;
    int h = src.rows;

    if (x > 0) {
        x1 = x; x2 = w;
        X1 = 0; X2 = w - x;
    }
    else {
        x1 = 0; x2 = w + x;
        X1 = -x; X2 = w;
    }
    if (y > 0) {
        y1 = y; y2 = h;
        Y1 = 0; Y2 = h - y;
    }
    else {
        y1 = 0; y2 = h + y;
        Y1 = -y; Y2 = h;
    }
    src.colRange(X1, X2).copyTo(dst.colRange(x1, x2));
    dst.rowRange(Y1, Y2).copyTo(temp.rowRange(0, Y2 - Y1));
    dst = cv::Mat::zeros(targetH, targetW, CV_16UC1);
    temp.rowRange(0, Y2 - Y1).copyTo(dst.rowRange(y1, y2));
    dst.copyTo(_dst);
}


void moveMat16Bit2(cv::Mat & src, cv::Mat & _dst, cv::Mat & temp, int targetW, int targetH, int x, int y) {
    cv::Mat dst = cv::Mat::zeros(targetH, targetW, CV_16UC1);
    temp = cv::Mat::zeros(targetH, targetW, CV_16UC1);
    int w = src.cols;
    int h = src.rows;

    int copyw =x>=0?MIN(targetW-x,w):w+x;
    int copyh =y>=0?MIN(targetH-y,h):h+y;

    int startX = x>=0? x:0;
    int startY = y>=0? y:0;

    int startx = x>=0?0:-x;
    int starty = y>=0?0:-y;

    cv::Rect roi_rect(startX,startY,copyw,copyh);
    src(cv::Rect(startx,starty,copyw,copyh)).copyTo(dst(roi_rect));
    dst.copyTo(_dst);
}


int strHex2Int(char* str, int length){
    int sum = 0;
    int k = length - 1;
    for (int i = 0 ;i < length; i++){
        if (str[i]>='0' && str[i]<='9') sum += (int(str[i])-48) * pow(16,k);
        else if (str[i]>='a' && str[i]<='f') sum += (int(str[i])-87) * pow(16,k);
        k--;
    }
    return sum;
}

int rebuildHimaxData(uchar* datagroup, int frameHeight, int frameWidth, short *irData, float* depthData) {
    //qDebug()<<datagroup[frameHeight*frameWidth * 2 - 1];
    memset(irData,0,frameWidth*frameHeight*sizeof(short));
    memset(depthData,0,frameWidth*frameHeight*sizeof(float));
    bool isDepth = false;
    if (datagroup[frameHeight*frameWidth * 2 - 1] == 6)
    {
        isDepth = true;
    }
    else if (datagroup[frameHeight*frameWidth * 2 - 1] == 1)
    {
        isDepth = false;
    }
    else
    {
        return -1;
    }
    uchar* tmp = datagroup;
    if (isDepth) {
        float* dst = depthData;
        //cv::Mat dst = imgList[1];
        for (int y = 0; y < frameHeight; y++) {
            for (int x = 0; x < frameWidth; x++) {
                short valLSB = tmp[x * 2];
                short valMSB = tmp[x * 2 + 1];

                short tmp0 = (valLSB >> 4);
                short tmp1 = (valMSB << 4);
                short tmp_frac = (valLSB & 15);
                dst[x] = 1.*tmp0 + 1.*tmp1 + 1.*tmp_frac / 16;
            }
            tmp += frameWidth * 2;
            dst += frameWidth;
        }
        return 0;
    }
    else {
        short* dst = irData;
        for (int y = 0; y < frameHeight; y++) {
            for (int x = 0; x < frameWidth; x++) {
                short valLSB = tmp[x * 2];
                short valMSB = tmp[x * 2 + 1];

                short tmp0 = ((valLSB & 192) >> 6);
                short tmp1 = (valMSB << 2);

                dst[x] = tmp0 + tmp1;
            }
            tmp += frameWidth * 2;
            dst += frameWidth;
        }
        return 1;
    }
}

bool yuv420ToRgb(char *yuv,int w,int h,char *rgb)
{
    unsigned char *pBufy = new unsigned char[w*h];
    unsigned char *pBufu = new unsigned char[w*h/4];
    unsigned char *pBufv = new unsigned char[w*h/4];

    memcpy(pBufy,yuv,w*h);
    memcpy(pBufu,yuv+w*h,w*h/4);
    memcpy(pBufv,yuv+w*h*5/4,w*h/4);

    for(int i = 0; i<w*h/4;i++)
    {
        rgb[i*3+2] = pBufy[i]+1.772*(pBufu[i]-128);  //B = Y +1.779*(U-128)
        rgb[i*3+1] = pBufy[i]-0.34413*(pBufu[i]-128)-0.71414*(pBufv[i]-128);//G = Y-0.3455*(U-128)-0.7169*(V-128)
        rgb[i*3+0] = pBufy[i]+1.402*(pBufv[i]-128);//R = Y+1.4075*(V-128)
    }
    delete[] pBufy;
    delete[] pBufu;
    delete[] pBufv;
    return true;
}

std::vector<std::string> splitString(std::string src, char split) {
    std::vector<std::string> reData;
    std::string tmp = "";
    for (int i = 0; i < src.size(); i++) {
        if (src[i] != split)
            tmp += src[i];
        else {
            reData.push_back(tmp);
            tmp = "";
        }
        if (i == src.size() - 1 && tmp != "") {
            reData.push_back(tmp);
        }
    }
    return reData;
}


void changeTime(long long ts,int &h,int &min, int &sec, int &us){
    h = ts / 3600000000;
    long long tmp = ts%3600000000;
    min = tmp / 60000000;
    tmp = tmp % 60000000;
    sec = tmp / 1000000;
    tmp = tmp % 1000000;
    us = tmp / 1000;
}

#endif // UTIL_HPP
