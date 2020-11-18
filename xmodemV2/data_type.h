#pragma once
#include<Windows.h>

#define c_comm_rece_size			512

typedef unsigned char	u8;
typedef char			s8;
typedef unsigned short	u16;
typedef short			s16;
typedef unsigned int	u32;
typedef int				s32;

typedef union
{
	u32 r;
	struct st_un32_b
	{
		u8 l;
		u8 m;
		u8 h;
		u8 u;
	}b;
	struct st_un32_w
	{
		u16 l;
		u16 h;
	}w;
}un32;


struct st_comm
{
	HANDLE		handle;
	int			num;
	int			open;
	u8			buf[c_comm_rece_size * 2];
	u8			rbuf[c_comm_rece_size * 2 + 1];
	u16			head;
	u16         tail;
	u32			tmr;
};

typedef struct st_app_comm
{
	struct st_comm comm;

	u32 xmodem_download_req;
	u32 xmodem_download_run;
	u32 xmodem_upload_req;
	u32 xmodem_upload_run;
	u8 *xmodem_download_addr;
	u32 xmodem_download_len;
	u8 *xmodem_upload_addr;
	u32 xmodem_upload_len;

	u8 *writeAddrLen_addr;
	u32 writeAddrLen_base;
	u16 writeAddrLen_len;
	u8 *readAddrLen_addr;
	u32 readAddrLen_base;
	u16 readAddrLen_len;

	u8 slave_irled_onoff;
	u8 slave_vcsel_onoff;
	u16 slave_current;
	u16 slave_duty;
}st_app;