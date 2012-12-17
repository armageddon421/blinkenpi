

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


void setpixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);


unsigned int width;

int i;

int main(void){
	
	width = 10;


	spi_init();	
		
	int i,t=0;

	while(42){
		
		for(i=0;i<LED_NUM/5;i++){
			led_set(i,0,0,0);	//245-152-157
		}	
		for(i=0;i<LED_NUM;i+=1){
			led_set(i,(sin(i*5.0+t/40.0)+1.0)*8.0,   (sin(i*8.0+t/30.0)+1.0)*8.0, ((i+t)%41 == 0 || (i+t)%39 == 0)?127:0);	
			
		}
		update();
		t++;
		//if(t>=10) t=0;
		usleep(1000);
	}

}


void setpixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b){
	
	unsigned int pos = y*width+(x%width);
	if (pos < LED_NUM){
		led_set(pos, r, g, b);
	}
}

