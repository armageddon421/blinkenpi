
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

void place_nom();

extern unsigned char buffer[];
int snake[LED_NUM];
int snakelen, snakeadd;
int nompos, snakenom;


int i;

double ctr;

int dir,pos;




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

	spi_init();

        initscr();                      /* Start curses mode              */
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        cbreak();
        noecho();


        
	snakesetup();

        while(42){
                in = getch();
                
		if (in == KEY_LEFT){
			dir = 0;
                }
                else if (in == KEY_RIGHT){
			dir = 1;
                }
                else if (in == KEY_UP){
			dir = 2;
                }
                else if (in == KEY_DOWN){
			dir = 3;
                }





		if(lastmove+20 <= ctr){
			lastmove = ctr;
			snake_do(dir);
                }

		
		



		
		mvprintw(0,2,"pos: %d\n", pos);
		refresh();
		
		snake_draw();
		buffer[nompos*3+1] = 127;
		
		blit();    
        	update();
		ctr++;
		}


        endwin();                       /* End curses mode                */

        return 0;


}


/*
int main(int argc, char *argv[]){
	
	WIDTH = 40;
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

	while(donum != 0){
		unsigned int tctr = 0;
		while (text[tctr]){
			draw_char((ctr-strlen(text)*6) + tctr*6,text[tctr]);
			tctr++;
		}

		//blank columns
		for (i=0;i<10;i++){
			unsigned int pos = (i*WIDTH+(39%WIDTH));
			buffer[pos*3+0] = 0;
			buffer[pos*3+1] = 0;
			buffer[pos*3+2] = 0;
			
			pos = (i*WIDTH+(0%WIDTH));
			buffer[pos*3+0] = 0;
			buffer[pos*3+1] = 0;
			buffer[pos*3+2] = 0;
			
			
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
*/

