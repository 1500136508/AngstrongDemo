/*
xmodem.h
*/
#include "utilcom.h"

#ifndef __xmodem_h__
#define __xmodem_h__

#define c_xmodem_sox        0x03    //  �Զ������ݳ��� 256B    �������ݰ��ĵ�һ�ֽڽ���
#define c_xmodem_stx        0x02    //<STX> 02H     1kB  xmodem
#define c_xmodem_soh        0x01    //<SOH> 01H     128B xmodem
#define c_xmodem_eot        0x04    //<EOT> 04H
#define c_xmodem_ack        0x06    //<ACK> 06H
#define c_xmodem_nak        0x15    //<NAK> 15H     ��NAK��������ʱУ��Ϊ�ۼӺ�
#define c_xmodem_nxk        0x16	//	�Զ������ݳ��� 256B
#define c_xmodem_can        0x18    //<CAN> 18H     ��'C'��������ʱУ��ΪCRC
#ifdef __cplusplus
extern "C" {
#endif
u32 xmodem_download(st_app *app);
u32 xmodem_upload(st_app *app);
#ifdef __cplusplus
}
#endif
#endif


