/*
comm.c
*/

#include "utilcom.h"

u32 comm_init(st_app *app)
{
	app->comm.handle = null;
	app->comm.num = 0;
	app->comm.open = 0;
	app->comm.head = app->comm.tail = 0;

	return c_ret_ok;
}

u32 comm_open(u32 comIndex, st_app *app)
{
	static u32 step = 0, tmr = 0;
	DCB dcb = { 0 };
	COMMTIMEOUTS timeouts;
	COMSTAT cs;
	DWORD dwError;
	u8 tbuf[32];
	wint_t commStr[32];
	u32 retLen, x;

	if (app->comm.open == 1) return c_ret_ok;

	switch (step)
	{
		case 0:
			app->comm.num = comIndex;
			printf("[comm open] %d\r\n", app->comm.num);

			__sprintf(tbuf, (app->comm.num >= 10) ? "\\\\.\\COM%d" : "COM%d", app->comm.num);		// L"COM5"
			char2wchar(tbuf, commStr);
			app->comm.handle = CreateFile(commStr,		//	COMxx
					GENERIC_READ | GENERIC_WRITE,		//	允许读和写
					0,									//	独占方式（共享模式）
					NULL,								//	安全属性
					OPEN_EXISTING,						//	打开而不是创建（创建方式）
					0,									//	同步模式
					NULL);								//	不使用模板
			if (app->comm.handle == INVALID_HANDLE_VALUE)
			{
				printf("[comm open] error:%d\r\n", GetLastError());	//	2     ERROR_FILE_NOT_FOUND
				return c_ret_nk;
			}

			dcb.BaudRate = 115200;						//波特率	
			dcb.ByteSize = 8;							//位数	
			dcb.Parity = NOPARITY;						//奇偶检验	
			dcb.StopBits = ONESTOPBIT;					//停止位数 	
			if (!SetCommState(app->comm.handle, &dcb))
			{
				printf("[comm open] error:%d\r\n", GetLastError());
				CloseHandle(app->comm.handle);
				return c_ret_nk;
			}
			if (!PurgeComm(app->comm.handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
			{
				printf("[comm open] error:%d\r\n", GetLastError());
				CloseHandle(app->comm.handle);
				return c_ret_nk;
			}
			if (!ClearCommError(app->comm.handle, &dwError, &cs))
			{
				printf("[comm open] error:%d\r\n", GetLastError());
				CloseHandle(app->comm.handle);
				return c_ret_nk;
			}

			if(!GetCommTimeouts(app->comm.handle, &timeouts))
			{
				printf("[comm open] error:%d\r\n", GetLastError());
				CloseHandle(app->comm.handle);
				return c_ret_nk;
			}

			timeouts.ReadIntervalTimeout = 30;			//	连续30毫秒没有收到新数据
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = 0;
			timeouts.WriteTotalTimeoutMultiplier = 0;
			timeouts.WriteTotalTimeoutConstant = 0;
			if(!SetCommTimeouts(app->comm.handle, &timeouts))
			{
				printf("[comm open] error:%d\r\n", GetLastError());
				CloseHandle(app->comm.handle);
				return c_ret_nk;
			}

			if(!SetupComm(app->comm.handle, c_comm_rece_size, c_comm_rece_size))
			{
				printf("[comm open] error:%d\r\n", GetLastError());
				CloseHandle(app->comm.handle);
				return c_ret_nk;
			}
			app->comm.open = 1;
			step = 0;
			return c_ret_ok;
			step = 1;
		break;

		case 1:
			app->comm.rbuf[0] = 0xaa;
			app->comm.rbuf[1] = 0xaf;
			app->comm.rbuf[2] = 0x00;
			app->comm.rbuf[3] = 0x04;
			app->comm.rbuf[4] = 0xbb;
			app->comm.rbuf[5] = 0x24;
			app->comm.rbuf[6] = 0xcf;
			app->comm.rbuf[7] = 0xa5;
			app->comm.rbuf[8] = (u8)(0xaa + 0xaf + 0x04 + 0xbb + 0x24 + 0xcf + 0xa5);
			if(comm_send(app->comm.rbuf, 9, app) == c_ret_ok)
			{
				tmr = tick_get();
				step = 2;
			}
			else if (tick_cmp(tmr, 100) == c_ret_ok)
			{
				CloseHandle(app->comm.handle);
				step = 0;
			}
		break;

		case 2:
			if (comm_read(&retLen, app) == c_ret_ok)
			{
				if (retLen >= 9)
				{
					for (x = 0; x < retLen - 8; x++)
					{
						if ((app->comm.rbuf[x + 0] == 0xaa)
							&& (app->comm.rbuf[x + 1] == 0xaa)
							&& (app->comm.rbuf[x + 2] == 0x00)
							&& (app->comm.rbuf[x + 3] == 0x04)
							&& (app->comm.rbuf[x + 4] == 0xfb)
							&& (app->comm.rbuf[x + 5] == 0x3c)
							&& (app->comm.rbuf[x + 6] == 0xaf)
							&& (app->comm.rbuf[x + 7] == 0xbc)
							&& (app->comm.rbuf[x + 8] == (u8)(0xaa + 0xaa + 0x04 + 0xfb + 0x3c + 0xaf + 0xbc)))
						{
							app->comm.open = 1;
							step = 0;
							return c_ret_ok;
						}
					}
				}
			}
			else if (tick_cmp(tmr, 100) == c_ret_ok)
			{
				CloseHandle(app->comm.handle);
				step = 0;
			}
		break;

		default:
			step = 0;
		break;
	}
	return c_ret_nk;
}

u32 comm_read(u32 *retLen, st_app *app)
{
	COMSTAT	ComStat;
	DWORD	dwErrorFlags;
	u32		x, y;
	u16		tail;

	ClearCommError(app->comm.handle, &dwErrorFlags, &ComStat);
	if (ComStat.cbInQue != 0)
	{
		x = (ComStat.cbInQue >= sizeof(app->comm.rbuf)) ? sizeof(app->comm.rbuf) : ComStat.cbInQue;
		if (ReadFile(app->comm.handle, app->comm.rbuf, x, &y, NULL) != 0)
		{
			for (x = 0; x < y; x++)
			{
				app->comm.buf[app->comm.head++] = app->comm.rbuf[x];
				if (app->comm.head >= sizeof(app->comm.buf))
					app->comm.head = 0;
				if (app->comm.head == app->comm.tail)
				{
					app->comm.tail++;
					if (app->comm.tail >= sizeof(app->comm.buf))
						app->comm.tail = 0;
				}
			}
		}
	}

	x = 0;
	tail = app->comm.tail;
	while (tail != app->comm.head)
	{
		app->comm.rbuf[x++] = app->comm.buf[tail++];
		if (tail >= sizeof(app->comm.buf))
			tail = 0;
	}
	if (x == 0) return c_ret_nk;

	app->comm.rbuf[x] = 0;
	*retLen = x;
	return c_ret_ok;
}

u32 comm_send(u8 *buf, u16 len, st_app *app)
{
	BOOL status;
	u32 retLen;

	status = WriteFile(app->comm.handle, buf, len, &retLen, NULL);
	if ((status != 0) && (retLen == len)) return c_ret_ok;
	return c_ret_nk;
}

u32 comm_close(st_app *app)
{
	if (app->comm.open == 1)
	{
		if (CloseHandle(app->comm.handle) != 0)
			app->comm.open = 0;
	}
	return c_ret_ok;
}

void comm_rx_clr(st_app *app)
{
	app->comm.tail = app->comm.head;
}

void comm_tail_add(u16 len, st_app *app)
{
	app->comm.tail = (app->comm.tail + len) % sizeof(app->comm.buf);
}

