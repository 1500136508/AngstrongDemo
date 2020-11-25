#include "imagereader.h"
#include "utilimage.hpp"
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

    datagroup = new uchar[frameWidth*frameHeight*4 + frameWidthR*frameHeightR*2];
    datagroupR = new uchar[frameWidthR*frameHeightR*2];
    irData = new unsigned char[frameWidth*frameHeight];
    irDataGamma = new unsigned char[frameWidth*frameHeight];
    depthData = new float[frameWidthR*frameHeightR];
    for(int i = 0; i < frameHeightR * frameWidthR;i++) depthData[i] = 500.0f;
    predepthData = new float[frameWidthR*frameHeightR];
    depthDataRGB = new float[frameWidthR*frameHeightR];
    tmpdepth = new float[frameWidthR*frameHeightR*4];
    _buf = new unsigned char[frameHeightR * frameWidthR * (3 * sizeof(int) + sizeof(uchar))];
    _buf2 = new unsigned char[frameHeightR * frameWidthR * (4 * sizeof(int) + sizeof(uchar))];

    camds = new CCameraDS();

    rgbT = irT = depthT = 0;

    startTime = stopTime = 0;
    poolDepth = new QThreadPool(0);
    poolDepth->setMaxThreadCount(1);
    QtConcurrent::run( this,&imageReader::buildDataThread);

	m_bIsSaveImage = false;//初始化保存数据信号
	m_MouseX = -1;
	m_MouseY = -1;
}

imageReader::~imageReader()
{
    release();
}

bool imageReader::OpenCamera(int index)
{
	bool bReturn = false;
	do 
	{
#ifndef EFE_FORMAT
		if (!camds->isOpened() && !camds->OpenCamera(index, frameWidth, 1200, true))
		{
#else
		//if (!camds->isOpened() && !camds->OpenCamera(camIndex, frameWidth, 480, true))
		if (!camds->isOpened())
		{
#endif
			//CloseCamera();
			if (!camds->OpenCamera(index, frameWidth, 480, true))
			{
				//qDebug() << "camera init failed";
				break;
			}
		}

		bReturn = true;
	} while (false);

	return bReturn;
}

void imageReader::CloseCamera()
{
	isRunning = false;
	Sleep(2000);
	if (camds->isOpened())
	{
		camds->CloseCamera();
	}
	else
	{

	}
}

bool imageReader::IsOpen() const
{
	return camds->isOpened();
}

void imageReader::Live()
{
	if (camds->isOpened())
	{
		isRunning = true;
	}
}

void imageReader::Pause()
{
	isRunning = false;
}

void imageReader::Stop()
{
	isRunning = false;
	//CloseCamera();
}

void imageReader::buildDataThread()
{
	try
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
			if (!isRunning)
			{
				Sleep(3);//优化cpu占用率
				continue;
			}
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
				Sleep(3);
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
			//cv::imwrite("test.tiff", RGBFrame);
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

				depthFrame = cv::Mat(cv::Size(frameHeightR, frameWidthR), CV_32FC1, depthDataRGB);
				cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
				dilate(depthFrame, depthFrame, element);
				memcpy(depthDataRGB, depthFrame.data, frameWidthR*frameHeightR * sizeof(float));
				depthFrame = dealDepthMapColor(depthDataRGB, frameHeightR, frameWidthR);
				depthFrame.copyTo(container[1]);
				//cv::imwrite("depth.tiff", depthFrame);
				depthGet = true;
				thisRoundIR = false;
			}
			else if (flag == 1)
			{
				memcpy(&irT, ptr, sizeof(long long));
				unsigned char* dst = irData;
				unsigned char* dst2 = irDataGamma;
				for (int y = 0; y < frameHeight; y++)
				{
					for (int x = 0; x < frameWidth; x++)
					{
						dst[x*frameHeight + (frameHeight - 1 - y)] = (unsigned char)(tmp[x] >> 8);
						dst2[x*frameHeight + (frameHeight - 1 - y)] = hash_grayscale[(unsigned char)(tmp[x] >> 8)];
					}
					tmp += frameWidth;
				}
				//判断图像数据是否正常，解决闪屏问题
				/*for (int i = 0; i < frameHeight*frameWidth; ++i)
				{
					if (i == frameHeight * frameWidth-1)
					{
						Sleep(3);
						continue;
					}
					if (*irData != '0')
					{
						break;
					}
					else
					{
						irData += i;
					}
				}*/
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
			irGet = depthGet = true;
			if (irGet && depthGet && getParam)
			{
				if (flag_rd)
				{
					cv::Mat plus = RGBFrame / 2 + depthFrame / 2;
					plus.copyTo(container[2]);
				}
				cv::hconcat(container, combineFrame);
				emit sendImage(combineFrame);
			}
			//发送深度信息
			if (m_MouseX >= 0 && m_MouseY >= 0)
			{
				int xhere = m_MouseX % frameHeightR;
				int yhere = m_MouseY % frameWidthR;
				if (m_MouseX >= frameHeightR)
				{
					emit SendLocationDepth(xhere, yhere, depthDataRGB[yhere*frameHeightR + xhere]);
				}
				else
				{
					emit SendLocationDepth(-1, -1, 0);
				}
			}
			//发送平均深度信息
			if (calcArea)
			{
				float avg0 = 0.0;
				float avg1 = 0.0;
				if (getFirstArea)
				{
					qDebug() << "real one" << realX1 << " " << realY1 << " " << realX2 << " " << realY2;
					float pointNum = 0;
					float allDepth = 0.0;
					for (int x = realX1; x <= realX2; x++)
					{
						for (int y = realY1; y <= realY2; y++)
						{
							float d = 0.0;
							if (!isWarp)
							{
								//d = depthData[x + y * frameHeightR];
								d = depthData[x + y * frameHeightR];
							}
							else
							{
								//d = depthDataRGB[x + y * frameHeightR];
								d = depthDataRGB[x + y * frameHeightR];
							}
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
				qDebug() <<"avg0="<< avg0 << " " <<"avg1="<< avg1;
				emit SendAvgDepth(avg0, avg1);
				//calcArea = false;
			}
			if (m_bIsSaveImage)
			{
				if (abs(rgbT - irT) < 34000 && abs(rgbT - depthT) < 34000) {
#ifndef KEEP_ORI
					emit sendSaveImageData(irFrameAlign, RGBFrame, depthDataRGB);
#else
					dsaver->storeData(irFrameAlign, RGBFrame, predepthData);
#endif
				}
			}

			clock_t t2 = clock();
			if (t2 - t1 < 34)
				Sleep(34 - t2 + t1);
			else Sleep(10);
			qDebug() << "ONE ROUND : " << t2 - t1;
			
		}
	}
	catch (cv::Exception &e)
	{
		const char *err_msg = e.what();
		return;
	}
}

int imageReader::setParam(float _fx, float _fy, float _cx, float _cy)
{
	fx = _fx;
	fy = _fy;
	cx = _cx;
	cy = _cy;
	return 0;
}

void imageReader::release()
{
    quitProgram = true;
	if (datagroup)
	{
		delete[] datagroup;
		datagroup = nullptr;
	}
	if (datagroupR)
	{
		delete[] datagroupR;
		datagroupR = nullptr;
	}
    if (irData)
    {
		delete[] irData;
		irData = nullptr;
    }
    if (depthData)
    {
		delete[] depthData;
		depthData = nullptr;
    }
    if (predepthData)
    {
		delete[] predepthData;
		predepthData = nullptr;
    }
    if (depthDataRGB)
    {
		delete[] depthDataRGB;
		depthDataRGB = nullptr;
    }
    if (tmpdepth)
    {
		delete[] tmpdepth;
		tmpdepth = nullptr;
    }
    if (_buf)
    {
		delete[] _buf;
		_buf = nullptr;
    }
    if (_buf2)
    {
		delete[] _buf2;
		_buf2 = nullptr;
    }
    
    CloseCamera();
}

bool imageReader::IsRunning() const
{
	return isRunning;
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
			return;
		}
		isRunning = true;
	}
	else if (camds->isOpened())
	{
	}
	//isRunning = !isRunning;
	Sleep(100);
	if (!isRunning)
	{
		//camds->CloseCamera();
	}
}

void imageReader::SetSaveImageStatus(bool bIsSaveImage)
{
	m_bIsSaveImage = bIsSaveImage;
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
    setParam(rgb_param.fxrgb,rgb_param.fyrgb,rgb_param.cxrgb,rgb_param.cyrgb);
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

void imageReader::ReceiveMouseInfo(int x, int y)
{
	m_MouseX = x;
	m_MouseY = y;
}

void imageReader::ReceiveAvgArea(int nIndxe, QRectF rect)
{
	int flag = nIndxe;
	int x1 = rect.topLeft().x();
	int y1 = rect.topLeft().y();
	int x2 = rect.bottomRight().x();
	int y2 = rect.bottomRight().y();

	if (flag == -1  || rect.width() <= 0 || rect.height()<=0)
	{
		calcArea = false;
		realX1 = realX1s = realX2 = realX2s = realY1 = realY1s = realY2 = realY2s = -1;
		getFirstArea = false;
		getSecondArea = false;
		return;
	}
	if (x2 < x1)
	{
		int temp = x2;
		x2 = x1;
		x1 = temp;
	}
	isWarp = true;
	if (flag == 0)
	{
		realX1 = ((x1 == 0) || (x1 == frameHeightR) || (x1 == frameHeightR * 2)) ? 0 : x1 % frameHeightR;
		realY1 = y1 % frameWidthR;
		realX2 = ((x2 == frameHeightR) || (x2 == frameHeightR * 2) || (x2 == frameHeightR * 3)) ? frameHeightR : x2 % frameHeightR;
		realY2 = y2 % frameWidthR;
		//isWarp = x1 < frameHeightR ? false : true;
		calcArea = true;
		getFirstArea = true;
	}
	else if (flag == 1)
	{
		realX1s = ((x1 == 0) || (x1 == frameHeightR) || (x1 == frameHeightR * 2)) ? 0 : x1 % frameHeightR;
		realY1s = y1 % frameWidthR;
		realX2s = ((x2 == frameHeightR) || (x2 == frameHeightR * 2) || (x2 == frameHeightR * 3)) ? frameHeightR : x2 % frameHeightR;
		realY2s = y2 % frameWidthR;
		calcArea = true;
		getSecondArea = true;
		if (isWarp && realX1s > frameHeightR)
		{
			getFirstArea = getSecondArea = false;
			calcArea = false;
			return;
		}
	}
}
