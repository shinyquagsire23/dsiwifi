/*
 * Copyright (c) 2021 Max Thomas
 * This file is part of DSiWifi and is distributed under the MIT license.
 * See dsiwifi_license.txt for terms of use.
 */

#ifndef _DSIWIFI_SDIO_H
#define _DSIWIFI_SDIO_H

#include "common.h"

typedef struct {
    u8 unk1[0x40];
    char ssid[0x80];
    u8 unk2[0x40];
    u8 pmk[0x20];
    char pass[0x80];
    u8 unk3[0x60];
} nvram_cfg;

#define F1_HOST_INT_STATUS      (0x400)
#define F1_CPU_INT_STATUS       (0x401)
#define F1_ERROR_INT_STATUS     (0x402)
#define F1_COUNTER_INT_STATUS   (0x403)
#define F1_MBOX_FRAME           (0x404)
#define F1_RX_LOOKAHEAD_VALID   (0x405)
#define F1_RX_LOOKAHEAD0        (0x408)
#define F1_RX_LOOKAHEAD1        (0x40C)
#define F1_RX_LOOKAHEAD2        (0x410)
#define F1_RX_LOOKAHEAD3        (0x414)
#define F1_INT_STATUS_ENABLE    (0x418)
#define F1_COUNT4               (0x450)

extern nvram_cfg sdio_nvram_configs[3];

u32 sdio_read_func1_u32(u32 addr);
void sdio_write_func1_u32(u32 addr, u32 val);

u8 sdio_read_func0_u8(u32 addr);
int sdio_write_func0_u8(u32 addr, u8 val);

u32 sdio_read_intern_word(u32 addr);
void sdio_write_intern_word(u32 addr, u32 data);

void sdio_mbox0_send_packet(u8 type, u8 ack_type, const u8* data, u16 len, u16 idk);
u16 sdio_mbox0_readpkt(void);

void data_send_pkt_idk(u8* pkt_data, u32 len);
void data_send_pkt(u8* pkt_data, u32 len);
void data_send_test(const u8* dst_bssid, const u8* src_bssid, u16 idk);

u32 sdio_host_interest_addr();

#endif // _DSIWIFI_SDIO_H
