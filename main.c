

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <math.h>

#include "rgblib.h"
#include "screenlib.h"


int i;

double ctr;

extern unsigned char buffer[];



#define STEP 0.3;


int main(int argc, char *argv[]){
	
	ctr = 0;
	
	spi_init();	
	
	char *text;

	int donum = -1;
	
	if (argc >= 2){
		text = argv[argc-1];
	}
	else{
		text = "29c3";
	}	
	
	if (argc == 3){
		donum = argv[1][0]-'0'+1;
		printf("doing only %d times\n", donum-1);
	}
	ctr=0;
	while (0){
		for (i=0;i<LED_NUM;i++){
			buffer[i*3+0] = 63*(0.0+sin(((double)ctr*8+i+32*(i%WIDTH))/50.0));
			buffer[i*3+1] = 63*(0.0+sin(((double)ctr*8+i+32*(i%WIDTH))/50.0+3.14159*2/3));
			buffer[i*3+2] = 63*(0.0+sin(((double)ctr*8+i+32*(i%WIDTH))/50.0+3.14159*4/3));
		}	
		ctr++;	
		blit();
		update();		
	}

	while(donum != 0){
		unsigned int tctr = 0;
		while (text[tctr]){
			draw_char((ctr-strlen(text)*6) + tctr*6,text[tctr]);
			tctr++;
		}

		blit();	
		update();
		
		
		ctr-= STEP;

		if (ctr <= 0){
			ctr = strlen(text)*6+WIDTH;
			if (donum > 0) donum--;

		}


		//if (strlen(text)*4<WIDTH){
		//	if (ctr > WIDTH-1) ctr = 0;
		//}
		//else{
		//	if (ctr > strlen(text)*4 + WIDTH) ctr = 0;
		//}
		
//usleep(3000);
	}
}


