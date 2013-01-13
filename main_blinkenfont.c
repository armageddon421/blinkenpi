

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

void draw_char(double xo, char c);
void setpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void aapixel(double x, double y, unsigned char r, unsigned char g, unsigned char b);

unsigned char buffer[LED_NUM*3];

unsigned int width;

int i;

double ctr;

char tr[] = {255,	0,	0,	255,	255,	0,	128	};
char tg[] = {0,		255,	0,	255,	0,	255,	64	};
char tb[] = {0,		0,	255,	0,	255,	255,	192	};

void blit(){
	for(i=0; i<LED_NUM;i++){
		led_set(i,buffer[i*3+0],buffer[i*3+1],buffer[i*3+2]);
		buffer[i*3+0] = 0;
		buffer[i*3+1] = 0;
		buffer[i*3+2] = 0;
	}

}



#define STEP 0.15;


int main(int argc, char *argv[]){
	
	width = 40;
	ctr = 0;
	
	spi_init();	
	
	char *text;
	
	if (argc == 2){
		text = argv[1];
	}
	else{
		text = "29c3";
	}	

	while(42){
		unsigned int tctr = 0;
		while (text[tctr]){
			draw_char((-ctr) + tctr*6,text[tctr]);
			tctr++;
		}	
		blit();	
		update();
		
		
		ctr+= STEP;
		//if (strlen(text)*4<width){
		//	if (ctr > width-1) ctr = 0;
		//}
		//else{
		//	if (ctr > strlen(text)*4 + width) ctr = 0;
		//}
		
//usleep(3000);
	}
}


void draw_char(double xo, char c){
	int ix, iy;
	for(iy=0;iy<10;iy++){
		for(ix=0;ix<6;ix++){
			if(ix+xo < width || 1){
				unsigned char active = font[c][iy] & (1<<(ix+2));
				if(active >1) active = 1;
				//aapixel((ix+xo)*1.15,iy,active*(c*c)%127,active*(5*c)%127,active*(44137*c)%127);
				aapixel((ix+xo)*1.1,iy, active*(tr[c%7]/2), active*(tg[c%7]/2), active*(tb[c%7]/2) );
				//printf("%d",active);
			}	
		}
		//printf("\n");
	}
}


double dist(double a, double b){
	return sqrt(a*a+b*b);
}

void aapixel(double x, double y, unsigned char r, unsigned char g, unsigned char b){
	
	while(x<0) x += width;
	double xf = x - ((int)x);
		
	//while(y<0) x += width;
	//double yf = y - ((int)y);
	
	setpixel((int)x, (int)y, r*(1.0-xf), g*(1.0-xf), b*(1.0-xf));
	setpixel((int)x+1, (int)y, r*xf, g*xf, b*xf);
	
		
	//setpixel((int)x, (int)y+1, r*(1.0-yf), g*(1.0-yf), b*(1.0-yf));
	//setpixel((int)x+1, (int)y+1, r*xf, g*xf, b*xf);

	
	//setpixel((int)x, (int)y, r*dist(int(x)-x,int(y)-y, g*xf, b*xf);
	
}

void setpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b){
	
	unsigned int pos = (y*width+(x%width));
	while(pos<0) pos += LED_NUM;
	if (pos < LED_NUM && pos >= 0){
		//led_set(pos, r, g, b);
		buffer[pos*3+0] += r;
		buffer[pos*3+1] += g;
		buffer[pos*3+2] += b;
	}
}

