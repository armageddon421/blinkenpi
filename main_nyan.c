

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


#define WIDTH 32

void draw_char(double xo, char c);
void setpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void aapixel(double x, double y, unsigned char r, unsigned char g, unsigned char b);
void readBMP(char* filename, char* bmp);

unsigned char buffer[LED_NUM*3];
unsigned char buffer1[LED_NUM*3];
unsigned char buffer2[LED_NUM*3];

int width;
int i,j;

double ctr;

char tr[] = {255,	0,	0,	255,	255,	0,	128	};
char tg[] = {0,		255,	0,	255,	0,	255,	64	};
char tb[] = {0,		0,	255,	0,	255,	255,	192	};

void blit(char* tmp, int offset){
	for(j=0; j<LED_NUM;j++){
		i=j;
		if( (j/WIDTH)%2) i = (j/WIDTH)*WIDTH+(WIDTH-j%WIDTH-1);
		
		led_set(i/WIDTH*WIDTH+(i%WIDTH+offset)%WIDTH,tmp[j*3+0],tmp[j*3+1],tmp[j*3+2]);
	}

}




int main(void){
	
	ctr = 0;
	int i = 0;
	
	spi_init();	
	
	char *text;

	printf("Welcome to Nyancat, pls launch the music\n");

	readBMP("nyan/nyblack2.bmp", buffer1);			
	readBMP("nyan/nyblack.bmp", buffer2);

	while(42) {
		if((i%2)==0) {
			blit(buffer1,i);
		} else {
			blit(buffer2,i);
		}
		update();
		usleep(100000);
		i++;

	}
}

void readBMP(char* filename, char* bmp) {

	FILE* f = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f);
	
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];
	int pixel_num = height*width*3;	

	//unsigned char bmp[LED_NUM*3];

	//printf("Width: %d\n", width);
	//printf("Height: %d\n", height);

	unsigned char data[pixel_num]; //Buffer for the file
	fread(data, sizeof(unsigned char), pixel_num, f); // read the rest of the data at once
	fclose(f);

	int i=0,x,y,r,g,b;
	
		while(i < pixel_num) {
			//printf("Reading pixel: %d\n", i);
			y=(9-i/(width*3));
			x=(i%(width*3))/3;
			b=data[i+0]/2;
			g=data[i+1]/2;
			r=data[i+2]/2;
			//printf("x: %d ", x);
			//printf("y: %d ", y);
			//printf("r: %d ", r);
			//printf("g: %d ", g);
			//printf("b: %d\n\n", b);
			if(x<WIDTH){
	        		bmp[(y*WIDTH+x)*3+0] = r;
				bmp[(y*WIDTH+x)*3+1] = g;
        			bmp[(y*WIDTH+x)*3+2] = b;
			}
			i+=3;
		}

}

void draw_char(double xo, char c){
	int ix, iy;
	for(iy=0;iy<10;iy++){
		for(ix=0;ix<6;ix++){
			double xpos = (ix+xo) * 1.1;
			if(xpos < width && xpos >= 0){
				unsigned char active = font[c][iy] & (1<<(ix+2));
				
				if(active >1) active = 1;
				
				aapixel((xpos),iy, active*(tr[c%7]/2), active*(tg[c%7]/2), active*(tb[c%7]/2) );
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

