/*---------------------------------------------------------------------------------

    Simple console print demo
    -- dovoto

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <nds/arm9/exceptions.h>
#include <stdio.h>
#include <fat.h>

#include "lwip/sockets.h"
#include "dsiwifi9.h"
#include "rpc.h"

int lSocket;
struct sockaddr_in sLocalAddr;

void appwifi_echoserver(void);
bool is_connected = false;

// Unlaunch boot params need an extra 0x200 bytes
__attribute__((section(".secure")))
__attribute__((weak))
const char __secure_area__[0xC00];

const char* unlaunchID = "AutoLoadInfo";

void unlaunchRomBoot(char* path) 
{
    if (strlen(path) > 0x103) {
        path[0x102] = 0;
    }

	memcpy((u8*)0x02000800, unlaunchID, 12);
	*(u16*)(0x0200080C) = 0x3F0;		// Unlaunch Length for CRC16 (fixed, must be 3F0h)
	*(u16*)(0x0200080E) = 0;			// Unlaunch CRC16 (empty)
	*(u32*)(0x02000810) = 0;			// Unlaunch Flags
	*(u32*)(0x02000810) |= BIT(0);		// Load the title at 2000838h
	*(u32*)(0x02000810) |= BIT(1);		// Use colors 2000814h
	*(u16*)(0x02000814) = 0x7FFF;		// Unlaunch Upper screen BG color (0..7FFFh)
	*(u16*)(0x02000816) = 0x7FFF;		// Unlaunch Lower screen BG color (0..7FFFh)
	memset((u8*)0x02000818, 0, 0x20+0x208+0x1C0);		// Unlaunch Reserved (zero)
	for (int i = 0; i < strlen(path); i++) 
	{
		((int16_t*)0x02000838)[i] = path[i];		// Unlaunch Device:/Path/Filename.ext (16bit Unicode,end by 0000h)
	}
	while (*(u16*)(0x0200080E) == 0) {	// Keep running, so that CRC16 isn't 0
        *(u16*)(0x02000814) -= 1;
		*(u16*)(0x0200080E) = swiCRC16(0xFFFF, (void*)0x02000810, 0x3F0);		// Unlaunch CRC16
	}

	DC_FlushRange((void*)0x02000800, 0x200);
}

void appwifi_log(const char* s)
{
    iprintf("%s", s);
}

void appwifi_connect(void)
{
    rpc_init();
    //appwifi_echoserver();
    is_connected = true;
}

void appwifi_reconnect(void)
{
    rpc_deinit();
    rpc_init();
    is_connected = true;
}

void appwifi_echoserver(void)
{
    lSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (lSocket < 0) goto fail_socket;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sLocalAddr.sin_port = htons(1234);
    
    if (bind(lSocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0) {
        close(lSocket);
        goto fail_bind;
    }

    if ( listen(lSocket, 20) != 0 ){
        close(lSocket);
        goto fail_listen;
    }
    
    return;

fail_socket:
    iprintf("socket failed\n");
    goto loop_forever;

fail_bind:
    iprintf("bind failed\n");
    goto loop_forever;

fail_listen:
    iprintf("listen failed\n");
    goto loop_forever;

loop_forever:
    while(1) {
        swiWaitForVBlank();
    }
}

void appwifi_echoserver_tick(void)
{
    int clientfd;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[256];
    int nbytes;

    fcntl(lSocket, F_SETFL, O_NONBLOCK);
    clientfd = accept(lSocket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
    if (clientfd > 0)
    {
        iprintf("echo_server: Accept %x\n", clientfd);
        do
        {
            nbytes = recv(clientfd, buffer, sizeof(buffer),0);
            iprintf("echo_server: Got %x bytes\n", nbytes);
            if (nbytes>0) send(clientfd, buffer, nbytes, 0);
        }
        while (nbytes>0);

        close(clientfd);
    }
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
    defaultExceptionHandler();

    consoleDemoInit();  //setup the sub screen for printing
    consoleDebugInit(DebugDevice_CONSOLE);

    DSiWifi_SetLogHandler(appwifi_log);
    DSiWifi_SetConnectHandler(appwifi_connect);
    DSiWifi_SetReconnectHandler(appwifi_reconnect);
    DSiWifi_InitDefault(true);
    
    if (!fatInitDefault()) {
        iprintf("Failed to init SD card!\nRPC may not work fully...\n");
    }
    
    //while (!is_connected){}
    //iprintf("Starting echo server...\n");
    
    u32 last_addr = DSiWifi_GetIP();

    int ip_draw_cnt = 0;

    while(1) {
        u32 addr = DSiWifi_GetIP();
        
        if (addr != 0 && addr != 0xFFFFFFFF)
        {
            rpc_tick();
            //appwifi_echoserver_tick();
        }
        
        u8 addr_bytes[4];
        
        memcpy(addr_bytes, &addr, 4);
        
        if (ip_draw_cnt++ > 100) {
            int lock = enterCriticalSection();
            iprintf("\x1b[s\x1b[0;0HCur IP: \x1b[36m%u.%u.%u.%u        \x1b[u\x1b[37m", addr_bytes[0], addr_bytes[1], addr_bytes[2], addr_bytes[3]);
            ip_draw_cnt = 0;
            leaveCriticalSection(lock);
        }

        if (addr != last_addr)
        {
            swiWaitForVBlank();
        }
        
        last_addr = addr;
        scanKeys();
        if (keysDown()&KEY_START) break;
        if (keysDown()&KEY_B) {
            iprintf("asdf\n");
        }
    }
    
    close(lSocket);
    
    while (1) {
        fifoSendValue32(FIFO_USER_01, 1);
    }

    return 0;
}
