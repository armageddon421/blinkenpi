
#include <ncurses.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <math.h>

#include "../rgblib.h"
#include "../screenlib.h"
#include "../../nunchuck/test.h"

#define XCENTER 131
#define YCENTER 121
#define JOYDEADZONE 50

void place_nom();


extern int joyX;
extern int joyY;
extern int accelX;
extern int accelY;
extern int accelZ;
extern int buttonC;
extern int buttonZ;

extern unsigned char buffer[];
int snake[LED_NUM];
int snakelen, snakeadd;
int nompos, snakenom;


int i;

double ctr;

int dir,pos;

int tick_delay;


void loose(){
	double bri = 1.0;

	while(bri>0.01){	
		for(i=0;i<LED_NUM;i++){
			buffer[i*3] = (unsigned char)(bri*127);
		}
		blit();
		update();
		usleep(10000);
		for(i=0;i<LED_NUM;i++){
			buffer[i*3] = 0;
		}
		blit();
		update();
		usleep(10000);
		
		bri/=1.2;	
	}
	
	
	
}

void snakesetup(){
	pos = WIDTH*4+4;
	dir = 0;
	snakelen = 3;
	snakeadd = 0; 
	snake[0] = WIDTH*4+4;
	snake[1] = WIDTH*4+5;
	snake[2] = WIDTH*4+6;
	snakenom = snakelen+1;
	place_nom();
}

bool isonsnake(int x){
	for (i=0;i<snakelen;i++){
		if(snake[i] == x) return true;
	}
	return false;
}

void place_nom(){
	
	int tmp;
	do{	
		tmp = rand() % LED_NUM;
	} while(isonsnake(tmp));
	nompos = tmp;
}

void snake_draw(){
	for (i=0;i<snakelen;i++){
		buffer[snake[i]*3] = 127;
	}
	if (snakenom <= snakelen){
		buffer[snake[snakenom]*3+1] += (unsigned char)(40.0*(1.0-((double)snakenom)/snakelen));
	}
}

void snake_do(int dir){



	if (dir == 0){
		pos--;
		if( (pos%WIDTH)==WIDTH-1) pos += WIDTH;
	}
	else if (dir == 1){
		pos++;
		if( (pos%WIDTH)==0) pos -= WIDTH;
	}
	else if (dir == 2){
		pos-=WIDTH;
	}
	else if (dir == 3){
		pos+=WIDTH;
	}
	if(pos<0) pos += LED_NUM;
	if(pos>=LED_NUM) pos -= LED_NUM;


	if(isonsnake(pos)){
		loose();
		snakesetup();
	}
	
	for(i=snakelen;i>0;i--){
		snake[i] = snake[i-1];
	}
	snake[0] = pos;	
	
	if(snakeadd >0){
		snakeadd--;
		snakelen++;
	}
	if (snakenom <= snakelen){
		snakenom++;
	}
	
	if(isonsnake(nompos)){
		place_nom();
		snakeadd+=2;
		snakenom=0;
	}	
	
}



#define STEP 0.15;
int main(void){

        int in;

	int ctr = 0, lastmove = 0;
	
	pos = 0;
	dir = 0;
	
	tick_delay = 20;
	
	spi_init();

        initscr();                      /* Start curses mode              */
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        cbreak();
        noecho();

	nunchuck_init();
        
	snakesetup();

        while(42){
                in = getch();
               
		while(nunchuck_read()) nunchuck_init();
		 
		if (in == KEY_LEFT || (joyX < XCENTER - JOYDEADZONE && joyX > 10)){
			dir = 0;
                }
                else if (in == KEY_RIGHT || joyX > XCENTER + JOYDEADZONE){
			dir = 1;
                }
                else if (in == KEY_UP || joyY > YCENTER + JOYDEADZONE){
			dir = 2;
                }
                else if (in == KEY_DOWN || (joyY < YCENTER - JOYDEADZONE && joyY > 10)){
			dir = 3;
                }




		/*
		if(lastmove+tick_delay <= ctr){
			lastmove = ctr;
			snake_do(dir);
			
			if(!buttonC && tick_delay > 0) tick_delay--; 
			if(!buttonZ && tick_delay < 30) tick_delay++; 
                }
		*/
		
		
				
		setpixel((accelX-512)/10+WIDTH/2,(accelY-512)/20+6,127,32,16);

		
		mvprintw(0,2,"pos: %d\n", pos);
		mvprintw(1,0,"joy X,Y: %d, %d\n", joyX, joyY);
		mvprintw(2,0,"Accel X,Y,Z: %d, %d, %d\n", accelX, accelY, accelZ);
		mvprintw(3,0,"Button C,Z: %d, %d\n", buttonC, buttonZ);
		refresh();
		
		//snake_draw();
		buffer[nompos*3+1] = 127;
		
		blit();    
        	update();
		ctr++;
		}


        endwin();                       /* End curses mode                */

        return 0;


}



