/********************************
author:huangruopu
date:2020/05/13
function: dsense algorithm
version:v1.0_190805
mark: first release version
1.depth estimation
2.temperature shift warp
3.depth to rgb register
4.filling hole and denoise

********************************/ 
#pragma once

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)
typedef unsigned char uchar;//8bit
typedef unsigned short ushort;//16bit
typedef unsigned __int64 uint64;//64bit
typedef struct faceRectMy
{
    int x;
    int y;
    int width;
    int height;
}faceRectIn;//人脸框的位置大小
typedef struct adaptiveParameter
{
    int SADWindowSize;                     //SAD窗口大小
    int minDisparity;                      //最小视差，一般为负数
    int numberOfDisparities;               //视差范围 = 最大视差+最小视差-1
    int textureThreshold;                  //纹理check 阈值
    int uniquenessRatio;                   //uni check 阈值
    short disp12MaxDiff;                   //LR check 阈值
    short lambdaAD;                        //SAD系数
    short lambdaCensus;                    //census系数
    int focalLength;                       //焦距
    float b;                               //基线宽度
    float Z;                               //参考图距离
    bool isLeftCam;                        //摄像头是否在左边，默认是false
    bool isfilled;                         //是否补洞
    bool isDenoise;                        //是否去噪
    int speckleWindowSize;                 //去噪程度，越大去噪越多
    int holeSize;                          //补洞程度，越大补洞越多
    int lines;                             //多行搜索
}adaptiveState;//计算深度算法可调整的参数
typedef struct ir_rgb_Parameter
{
    //ir和彩色摄像头内参
    float fxir;
    float fyir;
    float cxir;
    float cyir;
    float fxrgb;
    float fyrgb;
    float cxrgb;
    float cyrgb;

    //ir和彩色摄像头外参
    float R00;
    float R01;
    float R02;
    float R10;
    float R11;
    float R12;
    float R20;
    float R21;
    float R22;
    float T1;
    float T2;
    float T3;

}ir_rgb_State;//摄像头内外参
//计算深度接口
void __declspec(dllexport) dsense_interface
(
    unsigned char* imgRight,               //输入的参考图
    unsigned char* imgLeft,                //实时图片
    float* depth,                          //输出深度图
    adaptiveState& adaptivePara,           //可调整的参数
    faceRectIn& rectIn,                    //人脸框位置大小
    int width,                             //输入图像宽度
    int height,                             //输入图像高度
    short* disparity              //输出视差图
);
//温度漂移调整接口
void __declspec(dllexport) temper_warp_interface
(
    unsigned char* input,                  //输入散斑图
    unsigned char* output,                 //温漂调整后的散斑图
    unsigned char* mapx,                   //温度x方向漂移的参数，需要内存大小width*height字节
    unsigned char* mapy,                   //温度x方向漂移的参数，需要内存大小width*height字节
    int width,                             //输入图像宽度
    int height,                            //输入图像高度
    float temperature,                     //实时读取到的当前温度
    float temp_stand,                      //标定参数里面的常温
    float temp_max,                        //标定参数里面的最大温度
    float temp_min                         //标定参数里面的最小温度
);

//深度图与彩色图对齐接口
void __declspec(dllexport) depth2RGB
(
    float* inputdepth,                     //输入深度图
    float* outdepth,                       //输出对齐后的深度图
    float* tmpdepth,                       //中间buffer，需要内存大小4*width*height字节
    int width_rgb,                         //彩色图宽度
    int height_rgb,                        //彩色图高度
    int width_ir,                          //红外图宽度
    int height_ir,                         //红外图高度
    ir_rgb_State& para                     //摄像头内外参
);

//IR图与彩色图对齐接口
int __declspec(dllexport) ir2RGB
(
    unsigned char* input_ir,               //输入IR图
    float* inputdepth,                     //输出原始深度图
    int width_ir,                          //IR图宽度
    int height_ir,                         //IR图高度
    ir_rgb_State& para,                    //摄像头内外参
    faceRectIn& rectIn,                    //IR图人脸框位置
    int& result_x,                         //输出:x方向需要平移的像素，正数表示向右平移
    int& result_y                          //输出:y方向需要平移的像素，正数表示向下平移
);

//去除深度图噪点的接口
void __declspec(dllexport) denoise(
    float* img,                //ori depth map change to clean depth map
    short newVal,              //error depth,if more than it ,not a bad depth
    int maxSpeckleSize,        //the maximum of speckle  , if less than it, is a speckle
    short maxDiff,             //the maximum of difference , if the pixel difference of two adjacent point is less than it,two point is the same speckle region
    unsigned char* _buf,       //buffer size = width*height*(3*sizeof(int)+sizeof(uchar)) <= 20M
    int max_th,                //remove out depth 
    int width,                 //image width
    int height                 //image height
);

//深度图补洞的接口
void __declspec(dllexport) filling(
    unsigned char* guide,         // gray edge map
    float* img,                   //ori depth map change to filled depth map
    short newVal,                 //error depth,if more than it ,not a bad depth
    int maxSpeckleSize,           //the maximum of speckle  , if less than it, is a speckle
    int windows,                  //searching window for fill average value
    unsigned char* _buf,          //buffer size = width*height*(3*sizeof(int)+sizeof(uchar)) <= 13M
    int width,                    //image width
    int height                    //image height
);
