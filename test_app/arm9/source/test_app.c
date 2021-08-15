/*---------------------------------------------------------------------------------

	Simple console print demo
	-- dovoto

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <nds/arm9/exceptions.h>
#include <stdio.h>
#include <fat.h>

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
