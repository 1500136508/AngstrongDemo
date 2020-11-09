#include "imagereader.h"
#include "util.hpp"
#include <QDebug>
#include "../dense_tencent/dsense_interface.h"
#include <QDir>
#include <windows.h>
//#include <QtConcurrent>
#include <QtConcurrent/QtConcurrent>
#include <imgproc.hpp>
#include <core.hpp>
#include <ml.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

cv::Mat K_rgb = cv::Mat::zeros(3,3,CV_32FC1);
cv::Mat R = cv::Mat::zeros(3,3,CV_32FC1);
cv::Mat K_ir = cv::Mat::zeros(3,3,CV_32FC1);
cv::Mat T = cv::Mat::zeros(3,3,CV_32FC1);
cv::Mat tmpM = cv::Mat::zeros(3,3,CV_32FC1);

unsigned char hash_grayscale[]={0, 15, 22, 27, 31, 35, 39, 42, 45, 47, 50, 52, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 74, 76, 78, 79, 81, 82, 84, 85, 87, 88, 90, 91, 93, 94, 95, 97, 98, 99, 100, 102, 103, 104, 105, 107, 108, 109, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149, 150, 151, 152, 153, 153, 154, 155, 156, 157, 158, 158, 159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 168, 168, 169, 170, 171, 171, 172, 173, 174, 174, 175, 176, 177, 177, 178, 179, 179, 180, 181, 182, 182, 183, 184, 184, 185, 186, 186, 187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195, 196, 196, 197, 198, 198, 199, 200, 200, 201, 201, 202, 203, 203, 204, 205, 205, 206, 206, 207, 208, 208, 209, 210, 210, 211, 211, 212, 213, 213, 214, 214, 215, 216, 216, 217, 217, 218, 218, 219, 220, 220, 221, 221, 222, 222, 223, 224, 224, 225, 225, 226, 226, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245,
            245, 246, 246, 247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 254, 255};

ir_rgb_State rgb_param{0};

unsigned char PD[1024];
float realpd[30];



imageReader::imageReader(QObject *parent)
{
    // 初始化各种buffer
#ifdef EFE_FORMAT
    depthFrame = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_8UC3);
    irFrame = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_8UC3);
    RGBFrame = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_8UC3);
#else
    depthFrame = cv::Mat(cv::Size(frameHeight,frameWidth),CV_8UC3);
    irFrame = cv::Mat(cv::Size(frameHeight,frameWidth),CV_8UC3);
    RGBFrame = cv::Mat(cv::Size(frameHeight,frameWidth),CV_8UC3);
#endif
    for (int i = 0; i < 3 ;i++) container.push_back(cv::Mat());
    irFrame.copyTo(container[0]);
    depthFrame.copyTo(container[1]);
    RGBFrame.copyTo(container[2]);
    for (int i =0;i<10;i++)
        imageQueue[i] = new unsigned char[640*480*2];
    datagroup = new uchar[frameWidth*frameHeight*4 + frameWidthR*frameHeightR*2];
    datagroupR = new uchar[frameWidthR*frameHeightR*2];
    rebulidRgb = new uchar[frameWidthR*frameHeightR*3];
    irData = new unsigned char[frameWidth*frameHeight];
    irDataGamma = new unsigned char[frameWidth*frameHeight];
    depthData = new float[frameWidthR*frameHeightR];
    for(int i = 0; i < frameHeightR * frameWidthR;i++) depthData[i] = 500.0f;
    predepthData = new float[frameWidthR*frameHeightR];
    depthDataRGB = new float[frameWidthR*frameHeightR];
    tmpdepth = new float[frameWidthR*frameHeightR*4];
    _buf = new unsigned char[frameHeightR * frameWidthR * (3 * sizeof(int) + sizeof(uchar))];
    _buf2 = new unsigned char[frameHeightR * frameWidthR * (4 * sizeof(int) + sizeof(uchar))];

    for (int i = 0;i<10;i++){
        predepthBuffer[i] = new float[frameWidthR*frameHeightR];
        depthBuffer[i] = new float[frameWidthR*frameHeightR];
        cloudptsBuffer[i] = new float[frameWidthR*frameHeightR*3];
    }

    camds = new CCameraDS();
    dsaver = new dataSaver();

    // 建立保存文件夹
    QDir qdir;
    qdir.mkdir(QString::fromStdString(savePath));

    lwriter = new logWriter();

    rgbT = irT = depthT = 0;

    startTime = stopTime = 0;
    poolDepth = new QThreadPool(0);
    poolDepth->setMaxThreadCount(1);
    QtConcurrent::run( this,&imageReader::buildDataThread);
//    QtConcurrent::run( this,&imageReader::readImageThread);


}

imageReader::~imageReader()
{
    release();
}

void imageReader::run()
{
#ifndef EFE_FORMAT
    if (!camds->isOpened() && !camds->OpenCamera("4321","12D1",frameWidth,1200,true)){
#else
    if (!camds->isOpened() && !camds->OpenCamera("4321","12D1",frameWidth,480,true)){
#endif
        qDebug()<<"camera init failed";
        emit sendLostServer(true);
        return;
    }
    else if (camds->isOpened()){
        emit sendLostServer(false);
    }
    isRunning = !isRunning;
    Sleep(100);
    if (!isRunning) camds->CloseCamera();
    emit sendStopSignal(isRunning);
}

void imageReader::run(int camIndex)
{
#ifndef EFE_FORMAT
	  if (!camds->isOpened() && !camds->OpenCamera(camIndex, frameWidth, 1200, true))
	  {
#else
	  //if (!camds->isOpened() && !camds->OpenCamera(camIndex, frameWidth, 480, true))
	  if (!camds->isOpened())
	  {
#endif
	  	if (!camds->OpenCamera(camIndex, frameWidth, 480, true))
	  	{
	  		qDebug() << "camera init failed";
	  		emit sendLostServer(true);
	  		return;
	  	}
	  }
	  else if (camds->isOpened())
	  {
	  	emit sendLostServer(false);
	  }
	  isRunning = !isRunning;
	  Sleep(100);
	  if (!isRunning)
	  {
	  	//camds->CloseCamera();
	  }
	  emit sendStopSignal(isRunning);
}


void imageReader::combineData()
{

}

void imageReader::recvData(){}

void imageReader::readImageThread()
{
    bool getParam = false;
    while (!quitProgram) {
        if (!isRunning) continue;
        if (!camds->isOpened()) continue;
        //unsigned char* datagroupTmp = new unsigned char[640*480*2];
        qMutex.lock();
        if (queuefront == -1){
            queuefront = 0;
            queuetail = 0;
        }
        camds->readRawData(imageQueue[queuefront]);
        qDebug() << imageQueue[queuefront][frameHeight*frameWidth*2 - 1];
        if (!getParam){
            memcpy(PD,imageQueue[queuefront] + 640*480*2 - 1032,1024);
            readpdData();
        }
        queuefront+=1;
        if (queuefront == 10) queuefront = 0;
        if (queuefront == queuetail) queuetail += 1;
        if (queuetail == 10) queuetail = 0;
        qMutex.unlock();
        Sleep(33);
    }
}

void imageReader::buildDataThread()
{
	bool irGet = false;
	bool depthGet = false;
	bool getParam = false;
	int moveX = 45;
	int moveY = 105;
	int preMoveX = 45;
	int preMoveY = 105;
	faceRectIn rectIn{ 130,195,170,215 };
	long long irT = 0;
	long long rgbT = 0;
	long long depthT = 0;
	long long lastRgbT = 0;
	while (!quitProgram)
	{
		try
		{
			if (!isRunning) continue;
			memset(datagroup, 0, 640 * 480 * 2);
			camds->readRawData(datagroup);
			clock_t t1 = clock();
			if (!getParam)
			{
				memcpy(PD, datagroup + 640 * 480 * 2 - 1032, 1024);
				readpdData();
				getParam = true;
			}
			bool thisRoundIR = true;
			uchar* ptr = datagroup + frameHeight * frameWidth * 2;
			memcpy(&rgbT, datagroup + 640 * 480 * 2 - 8, sizeof(long long));
			if (rgbT == lastRgbT)
			{
				continue;
			}
			else
			{
				lastRgbT = rgbT;
			}
			cv::Mat rgbyuv(cv::Size(frameWidthR, frameHeightR), CV_8U, ptr);
			//write data
			/*std::ofstream out("datagroup.bin", std::ofstream::binary);
			if (out.fail())
			{
				std::cout << "failed to open file" << std::endl;
				return;
			}*/
			//std::ostringstream out;
			//for (size_t r = 0; r < rgbyuv.rows;++r)
			//{
			//	const int *ptr = rgbyuv.ptr<int>(r);
			//	for (size_t c = 0;  c< rgbyuv.cols; c++)
			//	{
			//		//std::cout << rgbyuv.at<int>(i, j) << " ";
			//		//int temp = rgbyuv.at<int>(i, j);
			//		int temp = ptr[c];
			//		std::string strTemp;
			//		strTemp = std::to_string(temp);
			//		out << strTemp<<" ";
			//	}
			//	out << std::endl;
			//}
			//out.write((char*)rgbyuv.data, rgbyuv.cols*rgbyuv.elemSize());
			/*for (int r = 0; r < rgbyuv.rows; r++)
			{
				out.write(reinterpret_cast<const char*>(rgbyuv.ptr(r)), rgbyuv.cols*rgbyuv.elemSize());
			}*/
			/*for (int i = 0; i < 640 * 480 * 2;++i)
			{
				out.write((const char*)datagroup, 640 * 480 * 2);
				out.close();
				datagroup++;
			}*/

			/*	FILE* file = fopen("data_rgb.bin", "wb");
				if (file == NULL || rgbyuv.empty())
					return;
				fwrite("CmMat", sizeof(char), 5, file);
				int headData[3] = { rgbyuv.cols, rgbyuv.rows, rgbyuv.type() };
				fwrite(headData, sizeof(int), 3, file);
				fwrite(rgbyuv.data, sizeof(char), rgbyuv.step * rgbyuv.rows, file);
				fclose(file);*/

				//out.close();
				//cv::imwrite("test.tiff", rgbyuv);
			RGBFrame = cv::imdecode(rgbyuv, CV_LOAD_IMAGE_COLOR);
			cv::transpose(RGBFrame, RGBFrame);
			cv::flip(RGBFrame, RGBFrame, 0);
			cv::flip(RGBFrame, RGBFrame, -1);
			RGBFrame.copyTo(container[2]);
			//cv::imwrite("rgb.tiff", RGBFrame);
			ptr = datagroup;
			int flag = ptr[frameHeight*frameWidth * 2 - 1];
			unsigned short* tmp = (unsigned short*)ptr;
			if (flag == 2 || flag == 6)
			{
				memcpy(&depthT, ptr, sizeof(long long));
				float* dst = depthData;
				for (int y = 0; y < frameHeight; y++)
				{
					for (int x = 0; x < frameWidth; x++)
					{
						dst[x*frameHeight + (frameHeight - 1 - y)] = (float)(tmp[x] >> 4);
					}
					tmp += frameWidth;
				}
				denoise(depthData, 0, 200, 80, _buf, 2000, frameHeight, frameWidth);
				filling(edge.data, depthData, 0, 300, 10, _buf2, frameHeight, frameWidth);

#ifdef KEEP_ORI
				memcpy(predepthData, depthData, frameHeight*frameWidth * sizeof(float));
#endif
				depth2RGB(depthData, depthDataRGB, tmpdepth, frameHeightR, frameWidthR, frameHeight, frameWidth, rgb_param);
				//            cv::imwrite("ori_ir.bmp",irFrame);
				//            cv::imwrite("rgbFrame.bmp",RGBFrame);
				//            ir2RGB(irFrame.data,depthData,frameHeight,frameWidth,rgb_param,rectIn,moveX,moveY);

				//            if ((moveX != 0||moveY!=0) &&( abs(moveX-preMoveX) > 10 || abs(moveY- preMoveY) > 10)){
				//                preMoveX = moveX;
				//                preMoveY = moveY;
				//            }
				//            else{
				//                moveX = preMoveX;
				//                moveY = preMoveY;
				//            }
				//            qDebug()<<moveX<<" "<<moveY;

				depthFrame = cv::Mat(cv::Size(frameHeightR, frameWidthR), CV_32FC1, depthDataRGB);
				cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
				dilate(depthFrame, depthFrame, element);
				memcpy(depthDataRGB, depthFrame.data, frameWidthR*frameHeightR * sizeof(float));
				depthFrame = dealDepthMapColor(depthDataRGB, frameHeightR, frameWidthR);
				depthFrame.copyTo(container[1]);
				//cv::imwrite("depth.tiff", depthFrame);
				depthGet = true;
				thisRoundIR = false;

				if (calcArea)
				{
					float avg0 = 0.0;
					float avg1 = 0.0;
					if (getFirstArea) {
						qDebug() << "real one" << realX1 << " " << realY1 << " " << realX2 << " " << realY2;
						float pointNum = 0;
						float allDepth = 0.0;
						for (int x = realX1; x <= realX2; x++)
						{
							for (int y = realY1; y <= realY2; y++)
							{
								float d = 0.0;
								if (!isWarp)
									d = depthData[x + y * frameHeightR];
								else
									d = depthDataRGB[x + y * frameHeightR];
								if (d > 0.0)
								{
									allDepth += d;
									pointNum += 1;
								}
							}
						}
						avg0 = allDepth / pointNum;
					}
					if (getSecondArea)
					{
						qDebug() << "real two" << realX1s << " " << realY1s << " " << realX2s << " " << realY2s;
						float pointNum = 0;
						float allDepth = 0.0;
						for (int x = realX1s; x <= realX2s; x++)
						{
							for (int y = realY1s; y <= realY2s; y++)
							{
								float d = 0.0;
								if (!isWarp)
									d = depthData[x + y * frameHeightR];
								else
									d = depthDataRGB[x + y * frameHeightR];
								if (d > 0.0)
								{
									allDepth += d;
									pointNum += 1;
								}
							}
						}
						avg1 = allDepth / pointNum;

					}
					qDebug() << avg0 << " " << avg1;
					emit sendAreaAvg(avg0, avg1);
					//calcArea = false;
				}
				if (!(mouseX == 0 && mouseY == 0) && (0 <= mouseX /*&& mouseX <= frameHeight */ && 0 <= mouseY /*&& mouseY<=frameWidth*/)) {
					int xhere = mouseX % frameHeightR;
					int yhere = mouseY % frameWidthR;
					//                if (mouseX<frameHeight)
					//                    emit sendLocationDepth(xhere,yhere,depthData[yhere*frameHeightR+xhere]);
					//                else
					//                    emit sendLocationDepth(xhere,yhere,depthDataRGB[yhere*frameHeightR+xhere]);
					//                qDebug()<<mouseX;
					if (mouseX >= frameHeightR)
						emit sendLocationDepth(xhere, yhere, depthDataRGB[yhere*frameHeightR + xhere]);
					else
						emit sendLocationDepth(-1, -1, 0);
				}
			}
			else if (flag == 1) {
				memcpy(&irT, ptr, sizeof(long long));
				unsigned char* dst = irData;
				unsigned char* dst2 = irDataGamma;
				for (int y = 0; y < frameHeight; y++) {
					for (int x = 0; x < frameWidth; x++) {
						dst[x*frameHeight + (frameHeight - 1 - y)] = (unsigned char)(tmp[x] >> 8);
						dst2[x*frameHeight + (frameHeight - 1 - y)] = hash_grayscale[(unsigned char)(tmp[x] >> 8)];
					}
					tmp += frameWidth;
				}
				irFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC1, irData);
				irFrame.copyTo(irFrame16bit);
				cv::cvtColor(irFrame, irFrame, cv::COLOR_GRAY2BGR);
				Canny(irFrame, edge_clear, 35, 70);
				GaussianBlur(edge_clear, edge_th, cv::Size(3, 3), 0.5);
				threshold(edge_th, edge, 10, 255, cv::THRESH_BINARY);

				irFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC1, irDataGamma);
				cv::cvtColor(irFrame, irFrame, cv::COLOR_GRAY2BGR);
				//            moveMat2(irFrame,irFrameAlign,temp,frameHeightR,frameWidthR,moveX,moveY);
				//            moveMat16Bit2(irFrame16bit,irFrame16bit,temp,frameHeightR,frameWidthR,moveX,moveY);
				//            float rN = 1.45;
				//            int rNWidth = frameHeightR * rN;int rNHeight = frameWidthR * rN;
				//            int cutWidth = (rNWidth - frameHeightR) / 2;int cutHeight = (rNHeight - frameWidthR) / 2;
				//            cv::resize(irFrameAlign,irFrameAlign,cv::Size(rNWidth,rNHeight));
				//            irFrameAlign = cv::Mat(irFrameAlign,cv::Rect(cutWidth, cutHeight, frameHeightR,frameWidthR));
				//            cv::resize(irFrame16bit,irFrame16bit,cv::Size(rNWidth,rNHeight));
				//            irFrame16bit = cv::Mat(irFrame16bit,cv::Rect(cutWidth, cutHeight, frameHeightR,frameWidthR));
				cv::warpPerspective(irFrame, irFrameAlign, tmpM, cv::Size(480, 848));
				irFrameAlign.copyTo(container[0]);
				//cv::imwrite("ir.tiff", irFrameAlign);
				irGet = true;
				thisRoundIR = true;
			}
			//        qDebug() << irT <<" " << depthT << " " << rgbT;
			if (irGet && depthGet && getParam) {
				if (flag_rd) {
					cv::Mat plus = RGBFrame / 2 + depthFrame / 2;
					plus.copyTo(container[2]);
				}
				cv::hconcat(container, combineFrame);
				emit sendImage(combineFrame);
			}
			if (isGoingToSaveData) {
				if (max_count != 0) {
					if (save_count == max_count) {
						isGoingToSaveData = false;
						emit sendSaveInfo("Save data Done!");
						save_count = 0;
					}
					else {
						emit sendSaveInfo("Save data " + QString::number(save_count) + "/" + QString::number(max_count));
					}
				}

				if (abs(rgbT - irT) < 34000 && abs(rgbT - depthT) < 34000) {
#ifndef KEEP_ORI
					dsaver->storeData(irFrameAlign, RGBFrame, depthDataRGB);
#else
					dsaver->storeData(irFrameAlign, RGBFrame, predepthData);
#endif
					//                lwriter->writeAlog("rgb",true,save_count,-1);
					//                if (thisRoundIR)
					//                    lwriter->writeAlog("ir",true,save_count,-1);
					//                else
					//                    lwriter->writeAlog("depth",true,save_count,-1);
					save_count += 1;
				}
				else {
					dsaver->storeAloneRGBData(RGBFrame, rgbT);
					//                lwriter->writeAlog("rgb",true,save_count,rgbT);
				}

			}
			else {
				//            lwriter->writeAlog("rgb",false,save_count,-1);
				//            if (thisRoundIR)
				//                lwriter->writeAlog("ir",false,save_count,-1);
				//            else
				//                lwriter->writeAlog("depth",false,save_count,-1);
			}
			clock_t t2 = clock();
			if (t2 - t1 < 34)
				Sleep(34 - t2 + t1);
			else Sleep(10);
			qDebug() << "ONE ROUND : " << t2 - t1;
		}
		catch (cv::Exception &e)
		{
			const char *err_msg = e.what();
			continue;
		}
	}
}

void imageReader::release()
{
    dsaver->releaseDateSaver();
    quitProgram = true;
    delete[] datagroup;
    delete[] datagroupR;
    delete[] rebulidRgb;
    delete[] irData;
    delete[] depthData;
    delete[] predepthData;
    delete[] depthDataRGB;
    delete[] tmpdepth;
    delete[] _buf;
    delete[] _buf2;
    for (int i = 0;i<10;i++){
        delete[] predepthBuffer[i];
        delete[] depthBuffer[i];
        delete[] cloudptsBuffer[i];
    }
    for (int i =0;i<10;i++)
        delete[] imageQueue[i];
    camds->CloseCamera();
    //udpSocket->deleteLater();
}

void imageReader::saveDataSlot(int flag)
{
    qDebug()<< "save data in PC";
    if (!isRunning){
        emit sendSaveInfo("No Running Now...");
        return ;
    }
    emit sendSaveInfo("Start to save data");
    isGoingToSaveData  = true;
    save_count = 0;
    max_count = flag;
}

void imageReader::saveModeSetSlot(int mode)
{
    save_mode = mode;
    dsaver->setSavePathAndMode(savepth,save_mode);
}

void imageReader::saveStopSlot()
{
    isGoingToSaveData = false;
}

void imageReader::savePathSlot(QString savePath)
{
    savepth = savePath;
    dsaver->setSavePathAndMode(savepth,save_mode);
    lwriter->setSavePath(savepth.toStdString());
}

void imageReader::acceptLocation(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

void imageReader::acceptArea(int x1, int y1, int x2, int y2, int flag)
{
    if (x1 == -1) {
        calcArea = false;
        realX1 = realX1s = realX2 = realX2s = realY1 = realY1s = realY2 = realY2s = -1;
        getFirstArea = false;
        getSecondArea = false;
        return;
    }
    if (x2 < x1) {
        int temp = x2;
        x2 = x1;
        x1 = temp;
    }
    if (flag == 0){
        realX1 = ((x1 == 0)|| (x1 == frameHeightR)|| (x1 == frameHeightR*2))? 0:x1 % frameHeightR;
        realY1 = y1 % frameWidthR;
        realX2 = ((x2 == frameHeightR)|| (x2 == frameHeightR*2)|| (x2 == frameHeightR*3))? frameHeightR:x2 % frameHeightR;
        realY2 = y2 % frameWidthR;
        isWarp = x1 < frameHeightR ? false:true;
        getFirstArea = true;
    }
    else if (flag == 1){
        realX1s = ((x1 == 0)|| (x1 == frameHeightR)|| (x1 == frameHeightR*2))? 0:x1 % frameHeightR;
        realY1s = y1 % frameWidthR;
        realX2s = ((x2 == frameHeightR)|| (x2 == frameHeightR*2)|| (x2 == frameHeightR*3))? frameHeightR:x2 % frameHeightR;
        realY2s = y2 % frameWidthR;
        getSecondArea = true;
        if (isWarp && realX1s > frameHeightR){
            getFirstArea = getSecondArea = false;
            calcArea = false;
            return;
        }
    }
    calcArea = true;
}

void imageReader::stopingProgram()
{
    quitProgram = true;
}

void imageReader::readpdData()
{
    unsigned char *ptr = PD + 16;
    int datalen = 30;
    int i = 0;
    while (i < datalen) {
        memcpy(&realpd[i], ptr, sizeof(float)); ptr += sizeof(float);
        qDebug()<<"param "<< i << " " << realpd[i];
        i++;
    }
#ifdef EFE_FORMAT
    float ratio = 1080.f/480.0f;
#else
    float ratio = 2;
#endif
    rgb_param.fxir = realpd[0] /2;
    rgb_param.fyir = realpd[1] /2;
    rgb_param.cxir = realpd[2] /2;
    rgb_param.cyir = realpd[3] /2;

    rgb_param.fxrgb = realpd[9]  /ratio;
    rgb_param.fyrgb = realpd[10] /ratio;
    rgb_param.cxrgb = realpd[11] /ratio;
    rgb_param.cyrgb = realpd[12] /ratio;

    K_rgb.at<float>(0,0) = rgb_param.fxrgb;
    K_rgb.at<float>(0,2) = rgb_param.cxrgb;
    K_rgb.at<float>(1,1) = rgb_param.fyrgb;
    K_rgb.at<float>(1,2) = rgb_param.cyrgb;
    K_rgb.at<float>(2,2) = 1;

    K_ir.at<float>(0,0) = rgb_param.fxir;
    K_ir.at<float>(0,2) = rgb_param.cxir;
    K_ir.at<float>(1,1) = rgb_param.fyir;
    K_ir.at<float>(1,2) = rgb_param.cyir;
    K_ir.at<float>(2,2) = 1;

    rgb_param.R00 = realpd[18];
    rgb_param.R01 = realpd[19];
    rgb_param.R02 = realpd[20];

    rgb_param.R10 = realpd[21];
    rgb_param.R11 = realpd[22];
    rgb_param.R12 = realpd[23];

    rgb_param.R20 = realpd[24];
    rgb_param.R21 = realpd[25];
    rgb_param.R22 = realpd[26];

    R.at<float>(0,0) = rgb_param.R00;
    R.at<float>(0,1) = rgb_param.R01;
    R.at<float>(0,2) = rgb_param.R02;
    R.at<float>(1,0) = rgb_param.R10;
    R.at<float>(1,1) = rgb_param.R11;
    R.at<float>(1,2) = rgb_param.R12;
    R.at<float>(2,0) = rgb_param.R20;
    R.at<float>(2,1) = rgb_param.R21;
    R.at<float>(2,2) = rgb_param.R22;

    rgb_param.T1 = realpd[27];
    rgb_param.T2 = realpd[28];
    rgb_param.T3 = realpd[29];

    T.at<float>(0, 2) = rgb_param.T1;
    T.at<float>(1, 2) = rgb_param.T2;
    T.at<float>(2, 2) = rgb_param.T3;
    tmpM = K_rgb*R*K_ir.inv() + K_rgb*T*K_ir.inv() / 600;
    dsaver->setParam(rgb_param.fxrgb,rgb_param.fyrgb,rgb_param.cxrgb,rgb_param.cyrgb);
    std::ofstream logFile;logFile.open("moudule_param.yaml");
    logFile<<"fx: "<<rgb_param.fxrgb<<std::endl;
    logFile<<"fy: "<<rgb_param.fyrgb<<std::endl;
    logFile<<"cx: "<<rgb_param.cxrgb<<std::endl;
    logFile<<"cy: "<<rgb_param.cyrgb<<std::endl;
    logFile<<"tx: "<<40<<std::endl;
    logFile<<"mind: "<<300<<std::endl;
    logFile<<"maxd: "<<2000<<std::endl;
    logFile<<"f0: "<<rgb_param.fxrgb<<std::endl;
    logFile.close();
}

void imageReader::setShowPic(int flag, bool enable)
{
    if(flag==0) flag_ir = enable;
    else if(flag == 1) flag_depth=enable;
    else if(flag == 2) flag_rgb = enable;
    else if(flag == 3) flag_rd = enable;

    if (!flag_ir && !flag_depth && !flag_rgb && !flag_rd) flag_ir = flag_depth = flag_rgb = flag_rd = true;

}

/*====================old code=======================*/
void imageReader::readXMLData(QString xmlFile)
{
    cv::FileStorage fp;
    cv::Mat data;
    fp.open(xmlFile.toStdString(), cv::FileStorage::READ);
    if (!fp.isOpened()) return;
    fp["cameraMatrixIR"] >> data;
    double* dd = (double*)data.data;
#ifdef EFE_FORMAT
    float ratio = 1080.f/480.0f;
#else
    float ratio = 2;//1080.0f/480.0f;
#endif
    rgb_param.fxir = dd[0] /2;
    rgb_param.fyir = dd[4] /2;
    rgb_param.cxir = dd[2] /2;
    rgb_param.cyir = dd[5] /2;

    fp["cameraMatrixRGB"] >> data;
    dd = (double*)data.data;
    rgb_param.fxrgb = dd[0] /ratio;
    rgb_param.fyrgb = dd[4] /ratio;
    rgb_param.cxrgb = dd[2] /ratio;
    rgb_param.cyrgb = dd[5] /ratio;

    fp["IR_RGB_R"] >> data;
    dd = (double*)data.data;
    rgb_param.R00 = dd[0];
    rgb_param.R01 = dd[1];
    rgb_param.R02 = dd[2];

    rgb_param.R10 = dd[3];
    rgb_param.R11 = dd[4];
    rgb_param.R12 = dd[5];

    rgb_param.R20 = dd[6];
    rgb_param.R21 = dd[7];
    rgb_param.R22 = dd[8];

    fp["IR_RGB_T"] >> data;
    dd = (double*)data.data;
    rgb_param.T1 = dd[0];
    rgb_param.T2 = dd[1];
    rgb_param.T3 = dd[2];


    fp.release();


}

void imageReader::saveDataThread()
{
    qDebug()<< "start to save data";
    QDir qd;
    std::time_t t = std::time(0);
    char buf[128] = { 0 };
    strftime(buf, 64, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string timeSTR = std::string(buf);

    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR));
    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR+"//rgb"));
    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR+"//ir"));
    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR+"//depth"));
    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR+"//pointcloud"));

//    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR+"//irori"));
//    qd.mkdir(QString::fromStdString(savePath+"//"+timeSTR+"//depthori"));

    unsigned char* _buf_t = new unsigned char[frameHeightR * frameWidthR * (3 * sizeof(int) + sizeof(uchar))];
    unsigned char* _buf_t2 = new unsigned char[frameHeightR * frameWidthR * (4 * sizeof(int) + sizeof(uchar))];
    cv::Mat saveMat;
    FILE *plyfile;
    int h,m,s,us;
    h = m = s= us =0;
    for (int i = 0 ;i<10;i++){

        changeTime(irTs[i],h,m,s,us);
        std::string Pth = savePath+"//"+timeSTR+"//ir//"+std::to_string(h)+"-" +std::to_string(m)+"-"+std::to_string(s)+"-"+std::to_string(us) + "_00000"+std::to_string(i)+".png";
        cv::cvtColor(IRDATABUFFER[i],IRDATABUFFER[i],cv::COLOR_RGB2GRAY);
        cv::imwrite(Pth,IRDATABUFFER[i]);

//        Pth = savePath+"//"+timeSTR+"//irori//00000"+std::to_string(i)+".png";
//        cv::imwrite(Pth,irFrameOriBuffer[i]);
        // save rgb
        //pass
        denoise(depthBuffer[i], 0, frameHeightR, 10, _buf_t, 2000, frameHeightR, frameWidthR);
        edge = cv::Mat::zeros(cv::Size(frameHeightR,frameWidthR),CV_8UC1);
        filling(edge.data, depthBuffer[i], 0, frameHeightR, 10, _buf_t2, frameHeightR, frameWidthR);
        changeTime(depthTs[i],h,m,s,us);
        Pth = savePath+"//"+timeSTR+"//depth//"+std::to_string(h)+"-" +std::to_string(m)+"-"+std::to_string(s)+"-"+std::to_string(us) + "_00000"+std::to_string(i)+".png";
        saveMat = cv::Mat(cv::Size(frameHeightR,frameWidthR),CV_16UC1);
        for (int x = 0; x < frameHeightR; x++) {
            for (int y = 0; y < frameWidthR; y++) {
                saveMat.at<short>(y, x) = int(depthBuffer[i][y*frameHeightR + x]);
            }
        }
        cv::imwrite(Pth,saveMat);

        Pth = savePath+"//"+timeSTR+"//pointcloud//"+std::to_string(h)+"-" +std::to_string(m)+"-"+std::to_string(s)+"-"+std::to_string(us) + "_00000"+std::to_string(i)+".txt";
        fopen_s(&plyfile, Pth.c_str(), "w");
        for (int x = 0; x < frameHeight; x++) {
            for (int y = 0; y < frameWidth; y++) {
                float z = depthBuffer[i][y*frameHeight + x];
                if (z < 10e-6 && z > -10e-6) continue;
                float x_rw = ((float)x - rgb_param.cxrgb) * z / rgb_param.fxrgb;
                float y_rw = ((float)y - rgb_param.cyrgb) * z / rgb_param.fyrgb;
                fprintf(plyfile, "%.6f %.6f %.6f\n", x_rw, y_rw, z);
            }
        }
        fclose(plyfile);

//        Pth = savePath+"//"+timeSTR+"//depthori//00000"+std::to_string(i)+".png";
//        saveMat = cv::Mat(cv::Size(frameHeight,frameWidth),CV_16UC1);
//        for (int x = 0; x < frameHeight; x++) {
//            for (int y = 0; y < frameWidth; y++) {
//                saveMat.at<short>(y, x) = int(predepthBuffer[i][y*frameHeight + x]);
//            }
//        }
//        cv::imwrite(Pth,saveMat);
//        changeTime(rgbTs[i],h,m,s,us);
        Pth = savePath+"//"+timeSTR+"//rgb//"+std::to_string(h)+"-" +std::to_string(m)+"-"+std::to_string(s)+"-"+std::to_string(us) + "_00000"+std::to_string(i)+".jpg";
        cv::imwrite(Pth,RGBDATABUFFER[i]);



    }
    delete [] _buf_t;
    delete [] _buf_t2;
    emit sendSaveDone();
     qDebug()<< "save done";
}

void imageReader::checkThread()
{
    while (1){
        if (isRunning){
            if (stopTime == 0) continue;
            //if (stopTime - startTime < 0) continue;
            if (abs(stopTime - startTime) > 200) {
                isRunning = false;
                //emit sendStopSignal();
            }
        }
    }
}

int imageReader::socketInteract(int flag)
{
    if (flag == 0){
        serverOK = true;
    }
    else if (flag == 1){
        time1 = clock();
    }
    else if (flag == 2){
        emit sendSaveDone();
    }
    else if (flag == 3){
        // 送出图像显示
        stopTime = clock();
        // qDebug()<<stopTime-time1;
        // combineData();
        // qDebug()<<clock()-stopTime;


        // memset(irData,0,frameWidth*frameHeight);
        // memset(depthData,0,frameWidth*frameHeight*4);
        startTime = clock();
    }
    else if (flag == 4){
        //qDebug()<<"[TIME] READ FRAME "<< clock()-time1;
        qDebug()<<"error happened";
        isRunning=false;
    }
    else if (flag == 5) {
        std::vector<std::string> frdata = splitString(std::string(frameRateData),' ');
        emit sendFrameRate(QString::fromStdString(frdata[1]),QString::fromStdString(frdata[2]),QString::fromStdString(frdata[3]));
    }

    return 0;
}
