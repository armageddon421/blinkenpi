

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
		
		for(i=0;i<LED_NUM;i++){
			led_set(i,0,0,0);	//245-152-157
		}
		update();
		usleep(100000);

		for(i=0;i<LED_NUM;i+=1){
			led_set(i,127,127,127);	
			
		}
		update();
		usleep(2000);
	}

}


void setpixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b){
	
	unsigned int pos = y*width+(x%width);
	if (pos < LED_NUM){
		led_set(pos, r, g, b);
	}
}

