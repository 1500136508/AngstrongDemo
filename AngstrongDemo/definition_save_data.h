#pragma once

enum EGrabMode
{
	EGrabMode_3Pix = 0,
	EGrabMode_4Pix,
	EGrabMode_Count,
};

enum ESaveMode
{
	ESaveMode_Default = 0,
	ESaveMode_User,
	ESaveMode_Count,
};

enum EAccessroyMode
{
	EAccessroyMode_none = 0,
	EAccessroyMode_hat,
	EAccessroyMode_glasses,
	EAccessroyMode_colorglasses,
	EAccessroyMode_mask,
	EAccessroyMode_Count,
};

enum EDistanceMode
{
	EDistanceMode_300 = 0,
	EDistanceMode_500,
	EDistanceMode_800,
	EDistanceMode_Count,
};

enum EFaceActionMode
{
	EFaceActionMode_none = 0,
	EFaceActionMode_close_eyes,
	EFaceActionMode_not_normal_eyes,
	EFaceActionMode_not_open_mouth,
	EFaceActionMode_not_normal_mouth,
	EFaceActionMode_right_eye_block,
	EFaceActionMode_left_eye_block,
	EFaceActionMode_nose_block,
	EFaceActionMode_mouth_block,
	EFaceActionMode_left_face_block,
	EFaceActionMode_right_face_block,
	EFaceActionMode_chin_block,
	EFaceActionMode_Count,
};