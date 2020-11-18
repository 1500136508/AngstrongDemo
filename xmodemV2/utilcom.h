#pragma once

#include "data_type.h"
#ifndef null
#define null			(void*)0
#endif

#define c_ret_ok		0
#define c_ret_nk		1
#define c_ret_xk		2
#define c_ret_tmo		3
#define c_ret_next		4
#define c_ret_exit		5
#define c_ret_done		6


#define c_xmodem_sox        0x03    //  自定义数据长度 256B    根据数据包的第一字节解析
#define c_xmodem_stx        0x02    //<STX> 02H     1kB  xmodem
#define c_xmodem_soh        0x01    //<SOH> 01H     128B xmodem
#define c_xmodem_eot        0x04    //<EOT> 04H
#define c_xmodem_ack        0x06    //<ACK> 06H
#define c_xmodem_nak        0x15    //<NAK> 15H     以NAK握手启动时校验为累加和
#define c_xmodem_nxk        0x16	//	自定义数据长度 256B
#define c_xmodem_can        0x18    //<CAN> 18H     以'C'握手启动时校验为CRC


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <apiset.h>
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>
#include <wchar.h>
#include "comm.h"



#ifdef __cplusplus
extern "C" {
#endif
	u32 tick_get(void);
	u32 tick_cmp(u32 tmr, u32 tmo);
	wint_t *char2wchar(u8 *src, wint_t *dst);
	//返回(x-y)的绝对值
	u8  __abs8(u8 x, u8 y);
	u16 __abs16(u16 x, u16 y);
	u32 __abs32(u32 x, u32 y);
	u8* __memset(u8 *buf, u8 dat, u32 len);
	u8* __memcpy(u8 *dst, const u8 *src, u16 len);
	u8  __memcmp(u8 *dst, const u8 *src, u16 len);
	u16 __strlen(u8  *buf);
	u8* __strcpy(u8 *buf, const u8  *str);
	u8* __strncpy(u8 *buf, const u8 *str, u16 len);
	u8* __strcat(u8 *buf, const u8 *str);
	u8* __strncat(u8 *buf, const u8 *str, u16 len);
	u8  __strcmp(u8 *buf, const u8 *str);
	u8  __strncmp(u8 *buf, const u8 *str, u16 len);
	u8* __strchr(u8 *buf, u8 chr);
	u8* __strrchr(u8 *buf, u8 chr);
	u8* __strstr(u8 *buf, const u8 * str);
	//字符与数字相互转化
	u8  __hex2asc(u8 dig);
	u8  __asc2hex(u8 asc);
	u8  __dec2asc(u8 dig);
	u8  __asc2dec(u8 asc);
	//字符串转化为数值
	u32 __str2dec(u8 *buf);
	u32 __str2hex(u8 *buf);
	u16 __hz2hex16(u8 *buf);  //单个汉字转化成数字，目地是消除警告

							 //字符串与数据相互转化
	void __str2bufhex(u8 *buf, const u8 *str);			//"102C"->0x10,0x2c
	void __bufhex2str(u8 *str, const u8 *buf, u8 len);	//0x10,0x2c->"102C"
	void __str2bufdec(u8 *buf, const u8 *str);			//"1024"->0x0a,0x18
	void __bufdec2str(u8 *str, const u8 *buf, u8 len);	//0x0a,0x18->"1024"
	u16   __sprintf(u8 *buf, const s8 *fmt, ...);
	u16   __vsprintf(u8 *buf, const s8 *fmt, va_list ap);

	//	在字符串str中统计字符chr个数
	u8 __strchrs(u8* str, u8 chr);
	//	输出第h个字符chr后一字符到第h+1个字符chr前一字符之间的字符个数
	u16 __strlen_ht(u8 *str, u8 chr, u8 h);
	//	复制第h个字符chr后一字符到第h+1个字符chr前一字符
	u8 __strncpy_ht(u8* buf, u8* str, u8 chr, u8 h, u16 maxLen);

	extern	const u8 digit_tab[];
#ifdef __cplusplus
}
#endif

