/*
 * Copyright (c) 2021 Max Thomas
 * This file is part of DSiWifi and is distributed under the MIT license.
 * See dsiwifi_license.txt for terms of use.
 */

#include "wifi_debug.h"

#include <nds.h>
#include <nds/interrupts.h>
#include <stdio.h>
#include <stdarg.h>

char _print_buffer[0x80] = {0};

void wifi_printf(char* fmt, ...)
{
    int lock = enterCriticalSection();
    va_list args;
    va_start(args, fmt);
    vsnprintf(_print_buffer, 0x80-1, fmt, args);
    va_end(args);

    wifi_ipcSendString(_print_buffer);
    leaveCriticalSection(lock);
}

void wifi_printlnf(char* fmt, ...)
{
    int lock = enterCriticalSection();
    va_list args;
    va_start(args, fmt);
    vsnprintf(_print_buffer, 0x80-2, fmt, args);
    strcat(_print_buffer, "\n");
    va_end(args);

    wifi_ipcSendString(_print_buffer);
    leaveCriticalSection(lock);
}

void wifi_ipcSendString(char* ptr) {
    int lock = enterCriticalSection();
    fifoSendDatamsg(FIFO_USER_02, strlen(ptr)+1, (u8*)ptr);
    fifoSendValue32(FIFO_USER_03, 1);
    //while (!fifoCheckValue32(FIFO_USER_03));
    //while (fifoGetValue32(FIFO_USER_03) != 0);
    leaveCriticalSection(lock);
}
