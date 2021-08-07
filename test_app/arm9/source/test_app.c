/*---------------------------------------------------------------------------------

	Simple console print demo
	-- dovoto

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <nds/arm9/exceptions.h>
#include <stdio.h>

#include "dsiwifi9.h"

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	touchPosition touch;
	
	defaultExceptionHandler();

	consoleDemoInit();  //setup the sub screen for printing

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
		}
	}
	
	while (1) {
	    fifoSendValue32(FIFO_USER_01, 1);
	}

	return 0;
}
