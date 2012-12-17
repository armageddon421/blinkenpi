

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

unsigned int width;

int i;

int main(void){
	
	width = 0;


	spi_init();	
	int ctr = 0;
	unsigned char r=0,g=0,b=0;
		
	while(42){
		for (i=0; i<(LED_NUM-1);i++){
			int j;
			for(j=0;j<10;j++){	
				led_set((i+j)%LED_NUM,0,0,0);
			}
			for(j=0;j<10;j++){	
				led_set((i+1+j)%LED_NUM,r,g,b);
			}
			update();
			ctr++;
			r = (sin(ctr/50.0d)+1)*23;
			g = (cos(ctr/50.0d)+1)*23;
			b = (sin(ctr/80.0d)+1)*23;
					
			usleep(5000);	
		}
	}	

}
