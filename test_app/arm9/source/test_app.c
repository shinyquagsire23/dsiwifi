/*---------------------------------------------------------------------------------

	Simple console print demo
	-- dovoto

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <nds/arm9/exceptions.h>
#include <stdio.h>

#include "dsiwifi9.h"

char tmp_print[0x80];

static void wifiDebugValue32Handler(u32 value, void* data)
{
    if (fifoGetDatamsg(FIFO_USER_02, 0x80, (u8*)tmp_print)) {
        tmp_print[0x7F] = 0;
        iprintf("%s", tmp_print);
        memset(tmp_print, 0, 0x80);
        //fifoSendValue32(FIFO_USER_03, 1);
    }
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	touchPosition touch;
	
	defaultExceptionHandler();

	consoleDemoInit();  //setup the sub screen for printing

    fifoSetValue32Handler(FIFO_USER_03, wifiDebugValue32Handler, 0);
    
    Wifi_InitDefault(true);

	while(1) {

		//touchRead(&touch);
		//iprintf("\x1b[10;0HTouch x = %04i, %04i\n", touch.rawx, touch.px);
		//iprintf("Touch y = %04i, %04i\n", touch.rawy, touch.py);

		swiWaitForVBlank();
		scanKeys();
		if (keysDown()&KEY_START) break;
		if (keysDown()&KEY_B) {
		    iprintf("asdf\n");
		    fifoSendValue32(FIFO_DSWIFI, 7);
		}
	}
	
	while (1) {
	    fifoSendValue32(FIFO_USER_01, 1);
	}

	return 0;
}
