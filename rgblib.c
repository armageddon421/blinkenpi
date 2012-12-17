

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <math.h>

#include <wiringPiSPI.h>
#include "rgblib.h"


//output buffer
unsigned char output[LED_NUM*3];


int spi_init(){
	if (wiringPiSPISetup (SPI_CHANNEL, 1000000) < 0){
		printf("SPI Setup error\n");
		return 1;
	}

	int i;	
	for(i=0;i<LED_NUM;i++){
		led_set(i,0,0,0);
	}
	update();
	return 0;
}

void led_set(int id, unsigned char r, unsigned char g, unsigned char b){

	output[id*3 + 0] = (r^255) & 0b01111111;
	output[id*3 + 1] = (g^255) & 0b01111111;
	output[id*3 + 2] = (b^255) & 0b01111111;
}



void update(){


	unsigned char temp[LED_NUM*3];
	unsigned char latch[6] = {255,255,255,255,255,255};

	memcpy(temp,output, LED_NUM*3);
	
	wiringPiSPIDataRW (SPI_CHANNEL, temp, LED_NUM*3) ;
	//usleep(100000);
	wiringPiSPIDataRW (SPI_CHANNEL, latch, 6) ;
}
