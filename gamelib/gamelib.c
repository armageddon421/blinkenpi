
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
#include <time.h>

#include "../rgblib.h"
#include "../screenlib.h"
#include "../../nunchuck/test.h"
#include "../effects/effects.h"

#define XCENTER 131
#define YCENTER 121
#define JOYDEADZONE 50

extern int joyX;
extern int joyY;
extern int accelX;
extern int accelY;
extern int accelZ;
extern int buttonC;
extern int buttonZ;

extern unsigned char buffer[];

FILE *logfile;

int i;


void gamelib_mainmenu(){
	int n,j;
	int t = 0;
	double d=0;
	char *text;

	time_t current_time;
	struct tm * time_info;
	char timeString[8];



	while(nunchuck_read()){
		nunchuck_init();
		//nunchuck_read();
		mvprintw(6,0,"nunchuck reinited\n");
	}
	while(buttonC && buttonZ){
		while(nunchuck_read()){
			nunchuck_init();
			//nunchuck_read();
			mvprintw(6,0,"nunchuck reinited\n");
		}
		
		if(t%800 < 100){
			text = "PLAY";
			d = 3.03;
		}
		else if(t%800 < 200) {
			text = "GAMES";
			d = 0.03;
		}	
		else{
			time(&current_time);
			time_info = localtime(&current_time);
			strftime(timeString, 8, "%H:%M", time_info);
			
			if(current_time%2){
				timeString[2] = ' ';
			}
			text = timeString;
			d=0.03;
		}
		
		n=0;
		while(text[n] && n < 6){
			draw_char(n*6+d, text[n]);
			n++;
		}
		blit();
		update();
		
		t++;
	}


}	

void loose2(){
	effect_reset();
	int j;
	for(j=0;j<snakelen*5+50; j++){
		if(j%5==0 && j<snakelen*5){
			effect_create(EFFECT_TYPE_STROBE , snake[j/5], -63-32*(sin(j/5)+1),0,-32*(sin(j/5+3.14159)+1), 2, j, 30); 
			effect_create(EFFECT_TYPE_FADEIN , snake[j/5], -63-32*(sin(j/5)+1),0,-32*(sin(j/5+3.14159)+1), 2, j, 30); 
			effect_create(EFFECT_TYPE_DELAY , snake[j/5], -63-32*(sin(j/5)+1),0,-32*(sin(j/5+3.14159)+1), 2, j+30, snakelen*5+50); 
		}
		snake_draw();
		effect_draw(j);
		blit();
		update();
		//usleep(10000);
	}
	calc_score();
	display_score();
	welcome();
}

void display_score(){
	int i,j, n;
	
	time_t current_time;
	struct tm * time_info;
	char timeString[20];
	
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, 20, "%d.%m.%y %H:%M:%S", time_info);
	
	fprintf(logfile, "time: %s, score: %d, snakelen: %d, ctr: %d, speed: %d\n", timeString, score, snakelen, ctr, tick_delay);
	
	effect_reset();
	for (j=0;j<400;j++){
		char buf[10];
		sprintf(buf, "%d", score);
		n=0;
		while(buf[n] && n < 6){
			draw_char(n*6+sin(j/20.0), buf[n]);
			n++;
		}
		blit();
		update();
	}
}

void snakesetup(){
	score = 0;
	ctr = 0;
	lastmove = 0;
	pos = WIDTH*4+4;
	dir = 0;
	nextdir = 0;
	snakelen = 3;
	snakeadd = 0; 
	snake[0] = WIDTH*4+4;
	snake[1] = WIDTH*4+5;
	snake[2] = WIDTH*4+6;
	snakenom = snakelen+1;
	place_nom();
	effect_reset();
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
		buffer[snake[i]*3] += 63+32*(sin(i)+1);
		buffer[snake[i]*3+2] += 32*(sin(i+3.14159)+1);
	}
	if (snakenom <= 3){
		buffer[snake[snakenom]*3+1] += (unsigned char)(40.0*(1.0-((double)snakenom)/3));
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
		loose2();
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
		effect_create(EFFECT_TYPE_EXPLOSION , nompos, 0,100,0, 3, ctr, 40); 
		place_nom();
		snakeadd+=2;
		snakenom=0;
	}	
	
}



#define STEP 0.15;
int main(void){

        int in;

	ctr = 0;
	lastmove = 0;
	
	pos = 0;
	dir = 0;
	nextdir = 0;
	
	tick_delay = 10;
	
	spi_init();

        initscr();                      /* Start curses mode              */
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        cbreak();
        noecho();

	nunchuck_init();
	effect_init(); 
      
	welcome();
 
	snakesetup();


	logfile = fopen("snakelog.txt","a+");
	fprintf(logfile, "snake inited\n");

        while(42){
                in = getch();
              	do{ 
			while(nunchuck_read()){
				nunchuck_init();
				//nunchuck_read();
				mvprintw(6,0,"nunchuck reinited\n");
			}
		}while(joyX < 10 || joyX == 255 || joyY < 10 || joyY == 255);
		if (dir != 1 && (in == KEY_LEFT || (joyX < XCENTER - JOYDEADZONE && joyX > 10 ))){
			nextdir = 0;
                }
                else if (dir != 0 && (in == KEY_RIGHT || (joyX > XCENTER + JOYDEADZONE && joyX < 240))){
			nextdir = 1;
                }
                else if (dir != 3 && (in == KEY_UP || joyY > YCENTER + JOYDEADZONE)){
			nextdir = 2;
                }
                else if (dir != 2 && (in == KEY_DOWN || (joyY < YCENTER - JOYDEADZONE && joyY > 10))){
			nextdir = 3;
                }

		if (joyX > 150){

		}


		if(lastmove+tick_delay <= ctr){
			lastmove = ctr;
			dir = nextdir;
			snake_do(dir);
			
			//if(!buttonC && tick_delay > 0) tick_delay--; 
			//if(!buttonZ && tick_delay < 30) tick_delay++; 
                }
		
		
				
		//setpixel((accelX-512)/10+WIDTH/2,(accelY-512)/20+6,127,32,16);

		
		mvprintw(0,2,"pos: %d\n", pos);
		mvprintw(1,0,"joy X,Y: %d, %d\n", joyX, joyY);
		mvprintw(2,0,"Accel X,Y,Z: %d, %d, %d\n", accelX, accelY, accelZ);
		mvprintw(3,0,"Button C,Z: %d, %d\n", buttonC, buttonZ);
		calc_score();
		mvprintw(4,5,"Score: %d     ", score);	
		refresh();  //ncurses
		
		snake_draw();
		effect_draw(ctr);
		buffer[nompos*3+1] = 127;
		
		blit();    
        	update();
		ctr++;
		}


        endwin();                       /* End curses mode                */

        return 0;


}



