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

void appwifi_echoserver_tick(void);
bool is_connected = false;

void appwifi_log(const char* s)
{
    iprintf("%s", s);
}

void appwifi_connect(void)
{
    rpc_init();
    appwifi_echoserver();
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
    iprintf("Starting echo server...\n");    

    while(1) {
        appwifi_echoserver_tick();

        u32 addr = DSiWifi_GetIP();
        u8 addr_bytes[4];
        
        memcpy(addr_bytes, &addr, 4);
        
        iprintf("\x1b[s\x1b[0;0HCur IP: \x1b[36m%u.%u.%u.%u        \x1b[u\x1b[37m", addr_bytes[0], addr_bytes[1], addr_bytes[2], addr_bytes[3]);

        swiWaitForVBlank();
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
