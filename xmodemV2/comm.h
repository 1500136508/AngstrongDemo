/*
comm.h
*/
#include "utilcom.h"
#ifndef __comm_h__
#define __comm_h__

#ifdef __cplusplus
extern "C" {
#endif


u32 comm_init(st_app *app);

u32 comm_open(u32 comIndex, st_app *app);

u32 comm_read(u32 *retLen, st_app *app);

u32 comm_send(u8 *buf, u16 len, st_app *app);

u32 comm_close(st_app *app);

void comm_rx_clr(st_app *app);

void comm_tail_add(u16 len, st_app *app);




#ifdef __cplusplus
}
#endif
#endif