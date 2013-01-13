

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
#include "font10.h"

#include "screenlib.h"

char tr[] = {255,	0,	0,	255,	255,	0,	128	};
char tg[] = {0,		255,	0,	255,	0,	255,	64	};
char tb[] = {0,		0,	255,	0,	255,	255,	192	};

unsigned char buffer[LED_NUM*3];

int i, j;

double ctr;


void blit(){
	for(j=0; j<LED_NUM;j++){
		i=j;
		if( (j/WIDTH)%2) i = (j/WIDTH)*WIDTH+(WIDTH-j%WIDTH-1);
		
		led_set(j,buffer[i*3+0],buffer[i*3+1],buffer[i*3+2]);
		buffer[i*3+0] = 0;
		buffer[i*3+1] = 0;
		buffer[i*3+2] = 0;
	}

}


void draw_char(double xo, char c){
	int ix, iy;
	for(iy=0;iy<12;iy++){
		for(ix=0;ix<7;ix++){
			double xpos = (ix+xo) * 1.0;
			if(xpos < WIDTH && xpos >= -1){
				unsigned char active = font[c][iy] & (1<<(ix+1));
				
				if(active >1) active = 1;
				
				aapixel((xpos),iy, active*(tr[c%7]/2), active*(tg[c%7]/2), active*(tb[c%7]/2) );
			}	
		}
		//printf("\n");
	}
}

//returns fraction of completion of text display
double draw_string(double ts, char *text){
	int i = 0;
	double offset = WIDTH - ts;
	while (text[i]){
		draw_char(i*FONT_WIDTH+offset, text[i]);
		i++;
	}
	return ts/(strlen(text)*FONT_WIDTH+WIDTH);

}


double dist(double a, double b){
	return sqrt(a*a+b*b);
}

void aapixel(double x, double y, unsigned char r, unsigned char g, unsigned char b){
	
	//prevent wrapping	
	if (x >= WIDTH) return;	
	if (x < 0) return;	
	if (y < 0) return;	
	if (y >= HEIGHT) return;	
	
	//while(x<0) x += WIDTH;
	double xf = x - ((int)x);
		
	//while(y<0) x += WIDTH;
	//double yf = y - ((int)y);
	
	if((((int)x+1)%WIDTH)-1 >= 0)
	setpixel((int)x, (int)y, r*(1.0-xf), g*(1.0-xf), b*(1.0-xf));
	if((((int)x)%WIDTH)+1 < WIDTH)
	setpixel((int)x+1, (int)y, r*xf, g*xf, b*xf);
	
		
	//setpixel((int)x, (int)y+1, r*(1.0-yf), g*(1.0-yf), b*(1.0-yf));
	//setpixel((int)x+1, (int)y+1, r*xf, g*xf, b*xf);

	
	//setpixel((int)x, (int)y, r*dist(int(x)-x,int(y)-y, g*xf, b*xf);
	
}

void setpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b){
	
	//prevent wrapping	
	if (x >= WIDTH) return;	
	if (x < 0) return;	
	if (y < 0) return;	
	if (y >= HEIGHT) return;	
	unsigned int pos = (y*WIDTH+x);
	if (pos < LED_NUM && pos >= 0){
		//led_set(pos, r, g, b);
		buffer[pos*3+0] += r;
		buffer[pos*3+1] += g;
		buffer[pos*3+2] += b;
	}
}

