/*
xmodem.c
*/
#include "utilcom.h"
#include "xmodem.h"

//	spi flash方发送启动请求,pc方发送数据


u32 xmodem_download(st_app* app)
{
	u32 sendIdx, x, idx, tmr, retLen, ttmr, nakCnt;
	u8 packetNum, sum, buf[128 + 4], *px, retAck;

	if (app->xmodem_download_req == 1)
	{
		app->xmodem_download_req = 0;
		app->xmodem_download_run = 1;
		comm_rx_clr(app);
	}
	if (app->xmodem_download_run == 0) return c_ret_nk;

	nakCnt = 0;
	ttmr = tick_get();
	printf("[xmodem_download] start:%dms\r\n", tick_get());

	px = app->xmodem_download_addr;
	sendIdx = idx = 0;
	packetNum = 1;
	retAck = c_xmodem_nak;
	int k = 1;
	int base = app->xmodem_download_len / 10;
	while (1)
	{
		if (sendIdx >= k * base) {
			printf("already upload / all  %d / %d  --- %d%% \n", sendIdx, app->xmodem_download_len, 10 * k);
			k++;
		}
		if (retAck == c_xmodem_ack)
		{
			packetNum++;
			sendIdx += sizeof(buf) - 4;
			if (sendIdx >= app->xmodem_download_len)
			{
				buf[0] = c_xmodem_eot;
				comm_send(buf, 1, app);
				app->xmodem_download_run = 0;

				printf("[xmodem_download] stop: %dms, use:%dms, nak:%d\r\n", tick_get(), tick_get() - ttmr, nakCnt);
				return c_ret_ok;
			}
		}

		idx = sendIdx;
		for (sum = 0, x = 3; x < sizeof(buf) - 1; x++)
		{
			if (idx >= app->xmodem_download_len)
				buf[x] = 0x1a;
			else
				buf[x] = px[idx++];
			sum += buf[x];
		}
		buf[0] = c_xmodem_sox;
		buf[1] = packetNum;
		buf[2] = 0xff - buf[1];
		buf[x] = sum;

		comm_rx_clr(app);
		int retry = 0;
		comm_send(buf, sizeof(buf), app);
		retAck = c_xmodem_nak;
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 11000) == c_ret_ok)
			{
				buf[0] = c_xmodem_can;
				comm_send(buf, 1, app);
				app->xmodem_download_run = 0;
				return c_ret_tmo;
			}
			else if (comm_read(&retLen, app) == c_ret_ok)
			{
				comm_rx_clr(app);
				if (retLen == 1)
				{
					if (app->comm.rbuf[0] == c_xmodem_can)
					{
						app->xmodem_download_run = 0;
						return c_ret_xk;
					}
					else if (app->comm.rbuf[0] == c_xmodem_ack)
					{
						retAck = c_xmodem_ack;
						break;
					}
					else if (app->comm.rbuf[0] == c_xmodem_nak)
					{
						nakCnt++;
						break;
					}
				}
			}
		}
	}
	return c_ret_nk;
}

//	pc方发送启动请求,pc方接收数据
u32 xmodem_upload(st_app* app)
{
	u32 receIdx, x, tmr, retLen, ttmr, nakCnt;
	u8 packetNum, sum, buf[128 + 4], *px, *py, retry;

	if (app->xmodem_upload_req == 1)
	{
		app->xmodem_upload_req = 0;
		app->xmodem_upload_run = 1;

		tmr = tick_get();
		buf[0] = c_xmodem_nxk;
		comm_rx_clr(app);
		comm_send(buf, 1, app);
	}
	if (app->xmodem_upload_run == 0) return c_ret_nk;

	nakCnt = 0;
	ttmr = tick_get();
	printf("[xmodem_upload  ] start:%dms\r\n", tick_get());

	px = app->xmodem_upload_addr;
	receIdx = retry = 0;
	packetNum = 1;
	int k = 1;
	int base = app->xmodem_upload_len / 10;
	while (1)
	{
		if (receIdx >= k * base) {
			printf("already download / all  %d / %d  --- %d%% \n", receIdx, app->xmodem_upload_len, k * 10);
			k++;
		}
		if (tick_cmp(tmr, 500) == c_ret_ok)
		{
			tmr = tick_get();
			retry++;
			if (retry >= 20)
			{
				buf[0] = c_xmodem_can;
				comm_send(buf, 1, app);
				app->xmodem_upload_run = 0;
				return c_ret_tmo;
			}
			else
			{
				nakCnt++;
				buf[0] = c_xmodem_nxk;
				comm_send(buf, 1, app);
			}
		}
		else if (comm_read(&retLen, app) == c_ret_ok)
		{
			if (retLen == 1)
			{
				if (app->comm.rbuf[0] == c_xmodem_can)
				{
					comm_rx_clr(app);
					app->xmodem_upload_run = 0;
					return c_ret_xk;
				}
				else if (app->comm.rbuf[0] == c_xmodem_eot)
				{
					comm_rx_clr(app);
					buf[0] = c_xmodem_ack;
					comm_send(buf, 1, app);
					app->xmodem_upload_run = 0;

					//py = app->xmodem_download_addr;
					//for (x = 0; x < app->xmodem_upload_len; x++)
					//{
					//	if (px[x] != py[x]) break;
					//}
					return c_ret_ok;
				}
			}
			else if (retLen >= 260)
			{
				comm_rx_clr(app);
				if (app->comm.rbuf[1] == 0xff - app->comm.rbuf[2])
				{
					py = app->comm.rbuf;
					for (sum = 0, x = 3; x < sizeof(buf) - 1; x++)
					{
						sum += py[x];
					}
					if ((sum == app->comm.rbuf[x]) && ((app->comm.rbuf[1] == packetNum) || (app->comm.rbuf[1] + 1 == packetNum)))
					{
						tmr = tick_get();
						retry = 0;
						buf[0] = c_xmodem_ack;
						comm_send(buf, 1, app);

						if (app->comm.rbuf[1] == packetNum)
						{
							for (x = 3; x < sizeof(buf) - 1; x++)
							{
								if (receIdx < app->xmodem_upload_len)
									px[receIdx++] = py[x];
							}
							packetNum++;
						}
						else
						{
							nakCnt++;
						}

					}
				}
			}
			else {
				printf("Download Error\n");

			}
		}
	}
}
