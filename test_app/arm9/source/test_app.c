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

void appwifi_log(const char* s)
{
    iprintf("%s", s);
}

void appwifi_connect(void)
{
    rpc_init();
}

void appwifi_reconnect(void)
{
    rpc_deinit();
    rpc_init();
}

void appwifi_echoserver(void)
{
    int lSocket;
    struct sockaddr_in sLocalAddr;
    
    lSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (lSocket < 0) goto fail;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sLocalAddr.sin_port = htons(1234);
    
    if (lwip_bind(lSocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0) {
        lwip_close(lSocket);
        goto fail;
    }

    if ( lwip_listen(lSocket, 20) != 0 ){
        lwip_close(lSocket);
        goto fail;
    }

    while (1)
    {
        int clientfd;
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);
        char buffer[256];
        int nbytes;

        clientfd = lwip_accept(lSocket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
        if (clientfd > 0)
        {
            iprintf("echo_server: Accept %x\n", clientfd);
            do
            {
                nbytes=lwip_recv(clientfd, buffer, sizeof(buffer),0);
                iprintf("echo_server: Got %x bytes\n", nbytes);
                if (nbytes>0) lwip_send(clientfd, buffer, nbytes, 0);
            }
            while (nbytes>0);

            lwip_close(clientfd);
        }
    }
    lwip_close(lSocket);

fail:
    iprintf("not sure what happened\n");
    while(1) {
        swiWaitForVBlank();
    }
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
    touchPosition touch;
    
    
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

    appwifi_echoserver();

    while(1) {

        //touchRead(&touch);
        //iprintf("\x1b[10;0HTouch x = %04i, %04i\n", touch.rawx, touch.px);
        //iprintf("Touch y = %04i, %04i\n", touch.rawy, touch.py);

        swiWaitForVBlank();
        scanKeys();
        if (keysDown()&KEY_START) break;
        if (keysDown()&KEY_B) {
            iprintf("asdf\n");
        }
    }
    
    while (1) {
        fifoSendValue32(FIFO_USER_01, 1);
    }

    return 0;
}
