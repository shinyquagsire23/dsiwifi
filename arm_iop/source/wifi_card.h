/*
 * Copyright (c) 2021 Max Thomas
 * Copyright (c) 2015-2016, Daz Jones
 * This file is part of DSiWifi and is distributed under the MIT license.
 * See dsiwifi_license.txt for terms of use.
 */

#ifndef _WIFI_CARD_H
#define _WIFI_CARD_H

#include "common.h"

#include "wifi_sdio.h"

// 3DS
//#define REG_SDIO_BASE ((void*)0x10122000)

// DSi
#define REG_SDIO_BASE ((void*)0x4004A00)

typedef enum {
    wifi_card_dev_wlan = 0
} wifi_card_device;

typedef struct {
    wifi_card_device device;

    wifi_sdio_ctx tmio;
} wifi_card_ctx;

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

extern nvram_cfg wifi_card_nvram_configs[3];

u32 wifi_card_read_func1_u32(u32 addr);
void wifi_card_write_func1_u32(u32 addr, u32 val);

u8 wifi_card_read_func0_u8(u32 addr);
int wifi_card_write_func0_u8(u32 addr, u8 val);

u32 wifi_card_read_intern_word(u32 addr);
void wifi_card_write_intern_word(u32 addr, u32 data);

void wifi_card_mbox0_send_packet(u8 type, u8 ack_type, const u8* data, u16 len, u16 idk);
u16 wifi_card_mbox0_readpkt(void);

void data_send_pkt_idk(u8* pkt_data, u32 len);
void data_send_pkt(u8* pkt_data, u32 len);
void data_send_test(const u8* dst_bssid, const u8* src_bssid, u16 idk);

u32 wifi_card_host_interest_addr();

void wifi_card_init(void);
void wifi_card_deinit(void);
bool wifi_card_initted();

int wifi_card_device_init(wifi_card_device device);
wifi_card_ctx* wifi_card_get_context(wifi_card_device device);

void wifi_card_switch_device(wifi_card_ctx* ctx);
void wifi_card_send_command(wifi_sdio_command cmd, u32 args);
void wifi_card_send_command_alt(wifi_sdio_command cmd, u32 args);

u16 wifi_card_read16(u16 reg);
u32 wifi_card_read32(u16 reg);

void wifi_card_write16(u16 reg, u16 val);
void wifi_card_write32(u16 reg, u32 val);

void wifi_card_mask16(u16 reg, u16 clear, u16 set);
void wifi_card_mask32(u16 reg, u32 clear, u32 set);

void wifi_card_setclk(u32 data);
void wifi_card_stop(void);

void wifi_card_send_ready();
void wifi_card_send_connect();

#endif
