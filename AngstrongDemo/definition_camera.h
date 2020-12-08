#pragma once

enum ECameraStatus
{
	ECameraStatus_Unknow = -1,
	ECameraStatus_NoCamera = 0,
	ECameraStatus_Open,
	ECameraStatus_Close,
	ECameraStatus_Live,
	ECameraStatus_Pause,
	ECameraStatus_Stop,
	ECameraStatus_Count,
};

enum EDisplayMode
{
	EDisplayMode_Unknow = -1,
	EDisplayMode_IR_Depth_RGB = 0,
	EDisplayMode_IR,
	EDisplayMode_Depth,
	EDisplayMode_RGB,
	EDisplayMode_IR_Depth_RGBAddDepth,
	EDisplayMode_Count,
};