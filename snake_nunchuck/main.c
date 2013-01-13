
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

int place_nom();
void display_score();
bool isonsnake(int x);
bool isonsnake2(int x, int len);
void welcome();


//extern struct effect eflist[EFFECT_BUFSIZE];

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

int distance[LED_NUM];
int distance2[LED_NUM];
int rawdistance[LED_NUM];
int ancestor[LED_NUM];
int ancestor2[LED_NUM];
int visited[LED_NUM];
int deadend[LED_NUM];
int nomreplaced;


FILE *logfile;

int i;

int ctr, lastmove;

int dir,pos;
int nextdir;

int tick_delay;
int score;

double avgaccel;

int bot, demo;



char *autostatus;
int dijkstramode;
int dijkstratarget;

int right(int fwd){
	//li re ho ru
	switch(fwd){
		case(0):
			return 2;
		case(1):
			return 3;
		case(2):
			return 1;
		case(3):
			return 0;
	}
	
	return fwd;
}

int left(int fwd){
	int i,res=fwd;
	for(i=0;i<3;i++){
		res = right(res);
	}
	return res;
}

int curtarget;

int autodir(){

	int cpos = nompos;
	int res = dir;
	int i;
	
	//if(distance[nompos] > distance2[nompos] || distance[nompos] > 9000 || distance2[nompos] <= 0){
	if(nomreplaced || (distance2[nompos] > 999 && (dijkstramode != 2 || distance[nompos] < 999)) || distance2[nompos]<=0 || pos == dijkstratarget){
		
			nomreplaced = 0;
			for(i=0;i<LED_NUM;i++){
				ancestor2[i] = ancestor[i];
				distance2[i] = distance[i];
			}
	}
	
	autostatus = "normal";
	dijkstramode = 0;	
	if(distance2[nompos] > 900){
		//tick_delay = 30;
		autostatus = "snake";
		cpos = snake[snakelen];
	}

	if(distance2[cpos] > 900){
		dijkstramode = 1;
		autostatus = "manage";
		
		if(dijkstratarget != -1 && rawdistance[dijkstratarget] >= 1 && rawdistance[dijkstratarget] < 999 && pos != dijkstratarget){
			cpos = dijkstratarget;
			autostatus = "manage inherited";
			dijkstramode = 2;
		}
		else{
			
			int max = 0;
			int mpos;
			int i;
			for(i=0;i<LED_NUM;i++){
				if(distance2[i] < 900 && rawdistance[i] > max){
					max = rawdistance[i];
					mpos = i;
				}
			}
			if(mpos != -1 && max >= 1){
				cpos = mpos;
				dijkstratarget = mpos;
			}
			else{
				cpos = nompos;
			}
		}
		
	}
	
	if (dijkstramode == 0) dijkstratarget = -1;
	
	curtarget = cpos;
	
	if(distance2[cpos] < 900){
		i=0;
		while(cpos != -1 && ancestor2[cpos] != snake[0] && i<400){
			cpos = ancestor2[cpos];
			i++;
		}
		
		if(ancestor2[cpos] == snake[0]){
			int i;
			for(i=0;i<4;i++){
				if(getdir(snake[0],i) == cpos){
					res = i;
					break;
				}
			}
		}
	}
	else{
		autostatus = "fallback";
		if(!isonsnake(getdir(pos,right(dir))) && !deadend[getdir(pos,right(dir))] ){
			res = right(dir);
		}
		else if(!isonsnake(getdir(pos,dir)) && !deadend[getdir(pos,dir)] ){
			res = dir;
		}
		else if(!isonsnake(getdir(pos,left(dir))) && !deadend[getdir(pos,left(dir))] ){
			res = left(dir);
		}
		if(!isonsnake(getdir(pos,right(dir))) ){
			res = right(dir);
		}
		else if(!isonsnake(getdir(pos,dir)) ){
			res = dir;
		}
		else{
			res = left(dir);
		}
		
	}


	return res;
}


void draw_path(){
	
	int cpos = ancestor2[curtarget];
	int i = 0;
	while(cpos != -1 && cpos != pos && i<400){
		buffer[cpos*3+1] += 10;
		cpos = ancestor2[cpos];
		i++;
	}
	
	
}

void dlog(char *text){

	FILE *dbgfile = fopen("dbglog.txt","a+");
	fprintf(dbgfile, "%s", text);
	fclose(dbgfile);
}

int d_shortest(){
	int min = 99999;
	int sho = -1;
	int i;
	
	for(i=0;i<LED_NUM;i++){
		if(!visited[i] && distance[i] < min){
			min = distance[i];
			sho = i;
		}
	}
	return sho;	
}

int getdir(int p, int dir){
	//le ri up dn	
	int res = p;
	switch(dir){
	case(0): 	//left
		res--;
		if (res%WIDTH == WIDTH-1) res += WIDTH;
		break;
	case(1): 	//right
		res++;
		if (res%WIDTH == 0) res -= WIDTH;
		break;
	case(2): 	//up
		res -= WIDTH;
		if (res <0) res+=LED_NUM;
		break;
	case(3): 	//down
		res += WIDTH;
		if (res >= LED_NUM) res -=LED_NUM;
		break;
	}
	
	return res;	
}

int* d_neighbors(int *res, int p, int len){
	
	int i;
	
	for(i=0;i<4;i++){
		
		if(!isonsnake2(getdir(p,i), len)){
			res[i] = getdir(p,i);
		}
		else {
			res[i] = -1;
		}
	}
	
}


int whereonsnake(int p){
	int i;
	for(i=0;i<snakelen;i++){
		if (snake[i] == p) return i;
	}
	return -1;	
}

iterate_deadend(int dpos, int ddir){
	int i;
	int dnum = 0;
	int next = ddir;
	int longest = 0;
	for(i=0;i<4;i++){
		if(isonsnake(getdir(dpos,i)) || deadend[getdir(dpos,i)]){
			dnum++;
			int tmp = whereonsnake(getdir(dpos,i));
			if(tmp >= longest){
				longest = tmp;
			}
		}
		if(!isonsnake(getdir(dpos,i)) && !deadend[getdir(dpos,i)]){
			next = i;
		}
	}
	if (dnum <=2 || dnum == 4) return;
	deadend[dpos] = longest;
	iterate_deadend(getdir(dpos,next),next);
}


int dijkstra(int start){
	int i;
	for (i=0; i<LED_NUM; i++){
		distance[i] = 99999;
		rawdistance[i] = 99999;
		ancestor[i] = -1;
		visited[i] = 0;
		deadend[i] = 0;
	}
	distance[start] = 0;
	rawdistance[start] = 0;
	
	int shortest = start;

	for(i=0;i<LED_NUM;i++){
		if(isonsnake(i) || deadend[i]) continue;
		int j, num, ddir;
		int dirs[4];
		int longest;
		num = 0;	
		d_neighbors(dirs,i, snakelen);
		for(j=0;j<4;j++){
			if ( (dirs[j] == -1 || deadend[getdir(i,j)]) && snake[0] != getdir(i,j) ){
				num++;
				int tmp = whereonsnake(getdir(i,j));
				if(tmp >= longest){
					longest = tmp;
				}
			}
			if (dirs[j] != -1 && !deadend[getdir(i,j)]){
				ddir = j;
			}
		}
		if(num == 3){
			iterate_deadend(i,ddir);
			deadend[i] = longest;
		}
	}

	while (shortest != -1){
		visited[shortest] = 1;
		
		int i;
		int cost = 12;
		int walls = 0, prewall;

		int dirs[4];
		d_neighbors(dirs,shortest, snakelen+1+snakeadd-rawdistance[shortest]);
		for(i=0;i<4;i++){
			if (dirs[i] == -1 && dirs[i] != snake[0]){   // && !deadend[getdir(shortest, dirs[i])]){
				if (walls == 1 && prewall == right(right(i)))
					walls++;
				else if (walls == 0){
					walls++;
					prewall = i;
				}else;
			}
		}
		
		if(walls == 1) cost = 1;
		if(walls == 2) cost = 64;
		if(isonsnake(shortest)) cost = 1;
		
		int j;
		for(j=0;j<4;j++){
			i=(j+dir)%4;
			if(dirs[i] != -1 && (deadend[getdir(shortest, dirs[i])] == 0 || rawdistance[shortest]+1 > snakelen-deadend[getdir(shortest, dirs[i])]) ){
				if(distance[shortest]+1 < distance[dirs[i]]){
					distance[dirs[i]] = distance[shortest]+cost;
					rawdistance[dirs[i]] = rawdistance[shortest]+1;
					ancestor[dirs[i]] = shortest;
				}
			}
		}
		
		
		shortest = d_shortest();
	}	
	
}

void snake_draw(float div){
	double add = ctr-lastmove;
	add /= tick_delay;
	for (i=0;i<snakelen;i++){
		buffer[snake[i]*3] += 40+24*(sin(i*0.6+add)+1)/div;
		buffer[snake[i]*3+2] += 34*(sin(i*0.6+3.14159+add)+1)/div;
	}
	if (snakenom <= 3){
		buffer[snake[snakenom]*3+1] += (unsigned char)(40.0*(1.0-((double)snakenom)/3))/div;
	}
}

double nunlen(){

	double a = (512-accelX);
	double b = (512-accelY);
	double c = (512-accelZ);

	return sqrt(a*a+b*b+c*c);


}


void countdown(int mode){

	int i;
	for (i=3;i>0;i--){
		
		draw_char(i*8+1,'0'+i);
		
		if(mode == 1){
			snake_draw(i*2+2);
			buffer[nompos*3+1] = 127/(i*2+2);
		}
		blit();
		update();
		
		usleep(1000*500);
	}
}

void addscore(int score){
	
	int selected = 0;
	int lastpressed = 0;
	char names[10][4];
	int scores[10];
	char buf[20];
	int i;
	int ani = 0;
	double scrollpos = 0.0;
	
	FILE *scorefile=fopen("scores.txt","rt");

		
	i=0;	
		
	while(fgets(buf, 20, scorefile) != NULL)
   	{	
		int j;
		for(j=0;j<3;j++){
	 		names[i][j] = buf[j];
		}
		names[i][3] = 0;
		sscanf (buf+3, "%d", &(scores[i]));
		i++;
   	}
	fclose(scorefile);
	
	int slot = -1;
	
	for(i=0;i<10;i++){
		if(score > scores[i]){
			slot = i;
			break;
		}
	}
	if(slot == -1){
		return;
	}
	
	for(i=9;i>slot;i--){
		scores[i] = scores[i-1];
		strncpy(names[i], names[i-1],3);
		names[i][3] = 0;
	}
	
	char newname[4] = "AAA";
	
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
		if (joyY-YCENTER < -JOYDEADZONE){
			if(!lastpressed){
				newname[selected]++;
				if(newname[selected] > 'Z') newname[selected] = 'A';
				lastpressed = 1;
			}
		}
		else if (joyY-YCENTER > JOYDEADZONE){
			if(!lastpressed){
				newname[selected]--;
				if(newname[selected] < 'A') newname[selected] = 'Z';
				lastpressed = 1;
			}
		}
		else if(joyX-XCENTER > JOYDEADZONE){
			if(!lastpressed){
				selected++;
				selected = selected % 3;
				lastpressed = 1;
			}
		}
		else if(joyX-XCENTER < -JOYDEADZONE){
			if(!lastpressed){
				selected--;
				if (selected < 0) selected += 3;
				lastpressed = 1;
			}
		}
		else lastpressed = 0;
		
		
		sprintf(buf, "%s", newname);	
		int i=0;
		while(buf[i]){
			draw_char(i*6+6, buf[i]);
			i++;
		}
		
		for(i=0;i<6;i++){
			setpixel(i+selected*6+6,0,127,127,0);
			setpixel(i+selected*6+6,HEIGHT-1,127,127,0);
		}
		blit();
		update();
		ani++;
	}
	
	scores[slot] = score;
	strcpy(names[slot],newname);
	
	
	scorefile=fopen("scores.txt","wt");
	for(i=0;i<10;i++){
		fprintf(scorefile, "%s%d\n", names[i], scores[i]);
		
		sprintf(buf, "%s%d\n", names[i], scores[i]);
		dlog(buf);
	}	
	
	
	fclose(scorefile);	
}


void halloffame(){
	
	int selected = 0;
	int lastpressed = 0;
	char names[10][4];
	int scores[10];
	char buf[20];
	int i;
	int ani = 0;
	double scrollpos = 0.0;
	
	int acctr = 0;
	
	FILE *scorefile=fopen("scores.txt","rt");

		
	i=0;	
		
	while(fgets(buf, 20, scorefile) != NULL)
   	{	
		int j;
		for(j=0;j<3;j++){
	 		names[i][j] = buf[j];
		}
		names[i][3] = 0;
		sscanf (buf+3, "%d", &(scores[i]));
		i++;
   	}
	fclose(scorefile);
		
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
		
		if (accelZ < 560) acctr++;
		else acctr = 0;	
		if (acctr > 50) return;
		
		if (joyY-YCENTER < -JOYDEADZONE){
			if(!lastpressed){
				selected++;
				selected = selected % 10;
				lastpressed = 1;
			}
		}
		else if (joyY-YCENTER > JOYDEADZONE){
			if(!lastpressed){
				selected--;
				if (selected < 0) selected += 10;
				lastpressed = 1;
			}
		}
		else lastpressed = 0;

		if(joyX-XCENTER > JOYDEADZONE){
			scrollpos += (joyX-XCENTER-JOYDEADZONE)/30.0;
			if(scrollpos > 6*5) scrollpos = 30.0;
		}
		if(joyX-XCENTER < -JOYDEADZONE){
			scrollpos += (joyX-XCENTER+JOYDEADZONE)/30.0;
			if(scrollpos < 0) scrollpos = 0.0;
		}
		
		
		sprintf(buf, "%s %d", names[selected], scores[selected]);	
		int i=0;
		while(buf[i]){
			draw_char(i*6+6-scrollpos, buf[i]);
			i++;
		}
		
		setpixel(2,0,20,20,0);
		setpixel(WIDTH-1-2,0,20,20,0);
		setpixel(scrollpos/30.0*(WIDTH-7)+3,0,120,120,0);
		
		setpixel(2,HEIGHT-1,20,20,0);
		setpixel(WIDTH-1-2,HEIGHT-1,20,20,0);
		setpixel(scrollpos/30.0*(WIDTH-7)+3,HEIGHT-1,120,120,0);
		
		setpixel(0,selected+1,127,127,0);
		setpixel(WIDTH-1,selected+1,127,127,0);
		
		blit();
		update();
		ani++;
	}

	
}


int settings(){
	
	int selected = 0;
	char names[4][8] = {"SOLO", "PLACE", "DEMO", "SCORE"};
	int lastpressed = 0;
	int acctr = 0;	
		
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
		
		if (accelZ < 560) acctr++;
		else acctr = 0;	
		if (acctr > 50){
			return 1;
		}
		
		
		if (joyX-XCENTER > JOYDEADZONE){
			if(!lastpressed){
				selected++;
				selected = selected % 4;
				lastpressed = 1;
			}
		}
		else if (joyX-XCENTER < -JOYDEADZONE){
			if(!lastpressed){
				selected--;
				if (selected < 0) selected += 4;
				lastpressed = 1;
			}
		}
		else lastpressed = 0;
	
		int i=0;
		while(names[selected][i]){
			draw_char(i*6+(WIDTH-strlen(names[selected])*6)/2, names[selected][i]);
			i++;
		}
		for(i=0;i<WIDTH;i++){
			setpixel(i,0,5,5,0);
			setpixel(i,HEIGHT-1,5,5,0);
		}
		for(i=1;i<HEIGHT-1;i++){
			setpixel(0,i,5,5,0);
			setpixel(WIDTH-1,i,5,5,0);
		}
		for(i=0;i<8;i++){
			setpixel(i+8*selected,0,100,100,0);
			setpixel(i+8*selected,HEIGHT-1,100,100,0);
		}


		blit();
		update();
	}
	if(selected == 0){
		demo = 0;
		bot = 0;
	}
	else if(selected == 1){
		demo = 0;
		bot = 1;
	}
	else if(selected == 2){
		demo = 1;
		bot = 1;
	}
	else if(selected == 3){
		usleep(1000*300);
		halloffame();
		usleep(1000*300);
		return settings();
	}
	return 0;	
}

void welcome(){
	do{
		int n,j;
		int t = 0;
		double d=0;
		char *text;
		int acctr = 0;

		time_t current_time;
		struct tm * time_info;
		char timeString[8];



		while(nunchuck_read()){
			nunchuck_init();
			//nunchuck_read();
			mvprintw(6,0,"nunchuck reinited\n");
		}
		while(buttonC && buttonZ && acctr < 50){
			while(nunchuck_read()){
				nunchuck_init();
				//nunchuck_read();
				mvprintw(6,0,"nunchuck reinited\n");
			}

			if(accelZ>600){
				acctr++;
			}
			else acctr = 0;
			
			if(t%800 < 100){
				text = "PLAY";
				d = 3.03;
			}
			else if(t%800 < 200) {
				text = "SNAKE";
				d = 0.03;
			}	
			else{
				time(&current_time);
				time_info = localtime(&current_time);
				strftime(timeString, 8, "%H:%M", time_info);
				
				if(current_time%2){
					timeString[2] = ' ';
				}
				if(current_time%200==0){
					bot = 1;
					demo = 1;
					return;	
				}
				text = timeString;
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
		demo = 0;
		bot = 0;
	}while(settings());
		
	countdown(0);

}	


void calc_score(){

	score = (32/(tick_delay+1)*ctr/(tick_delay+1)*snakelen/100+snakelen*1000)/10;


}


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

void loose2(){
	effect_reset();
	int j;
	for(j=0;j<snakelen*5+50; j++){
		if(j%5==0 && j<snakelen*5){
			effect_create(EFFECT_TYPE_STROBE , snake[j/5], -63-32*(sin(j/5)+1),0,-32*(sin(j/5+3.14159)+1), 2, j, 30); 
			effect_create(EFFECT_TYPE_FADEIN , snake[j/5], -63-32*(sin(j/5)+1),0,-32*(sin(j/5+3.14159)+1), 2, j, 30); 
			effect_create(EFFECT_TYPE_DELAY , snake[j/5], -63-32*(sin(j/5)+1),0,-32*(sin(j/5+3.14159)+1), 2, j+30, snakelen*5+50); 
		}
		snake_draw(1);
		effect_draw(j);
		blit();
		update();
		//usleep(10000);
	}
	calc_score();
	display_score();
	if(!bot && !demo){
		usleep(1000*300);
		addscore(score);
		usleep(1000*300);
	}
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
	
	
	char *typeString = "SOLO";
	if(bot && !demo) typeString = "PLACE";
	if(bot && demo) typeString = "DEMO";
	
	
	logfile = fopen("snakelog.txt","a+");
	fprintf(logfile, "%s time: %s, score: %d, snakelen: %d, ctr: %d, speed: %d\n", typeString, timeString, score, snakelen, ctr, tick_delay);
	fclose(logfile);
	
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
	avgaccel = nunlen();
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
	nompos = place_nom();
	nomreplaced = 1;
	effect_reset();

		for(i=0;i<LED_NUM;i++){
			ancestor2[i] = -1;
			distance2[i] = -1;
		}
}

bool isonsnake2(int x, int len){
	if (len > snakelen) len = snakelen;
	for (i=0;i<len;i++){
		if(snake[i] == x) return true;
	}
	return false;
}

bool isonsnake(int x){
	for (i=0;i<snakelen;i++){
		if(snake[i] == x) return true;
	}
	return false;
}

int place_nom(){

	if(demo == 0 && bot == 1){
		int selection = 0;
		double ax = 0;
		double ay = 0;
		do{
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
				ax = (ax*9+((accelX-512.0)/10+WIDTH/2))/10;
				ay = (ay*9+((accelY-512.0)/15+4))/10;
				int x = ax;
				int y = ay;

				if(x<0) x=0;
				if(y<0) y=0;
				if(x>=WIDTH) x=WIDTH-1;
				if(y>=HEIGHT) y=HEIGHT-1;
				
				selection = x+y*WIDTH;
				snake_draw(2);
				setpixel(x,y,5,127,5);
				blit();
				update();
			}
		}while(isonsnake(selection));
		dijkstra(snake[0]);
		return selection;
	}
	else{


		int tmp = rand() % LED_NUM;
		if (!isonsnake(tmp) && (rand() % 100) > 20 )
			return tmp;

		dijkstra(snake[0]);

		int max = 3;
		int newpos = -1;
		int i;
		
		for (i=0; i<LED_NUM; i++){
			if(!isonsnake(i) && distance[i] > max && (distance[i] < 99998 || (rand() % 100) > 80  )){
				max = distance[i];
				newpos = i;
			}
			
		}
			
		return newpos;
	}
}


void snake_do(int dir){



	if (dir == 0){
		pos--;
		if( (pos%WIDTH)==WIDTH-1 || pos < 0) pos += WIDTH;
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
		effect_create(EFFECT_TYPE_EXPLOSION , nompos, 0,20,0, 3, ctr, 40); 
		nompos = place_nom();
		nomreplaced = 1;
		snakeadd+=2;
		snakenom=0;
	}	
	
}

void snakepause(){
	
	int acctr = 0;
	
	while(buttonC && buttonZ && acctr < 50){
		while(nunchuck_read()){
			nunchuck_init();
			//nunchuck_read();
			mvprintw(6,0,"nunchuck reinited\n");
		}
		
		if(accelZ>600){
			acctr++;
		}
		else acctr = 0;
		
		char text[] = "PAUSE";
		int i;
		for (i=0; i<strlen(text); i++){
			draw_char(i*6+1,text[i]);
		}
		blit();    
        	update();
		
		
	}	
	countdown(1);	
}

#define STEP 0.15;
int main(void){
	
	autostatus = "init";
	
	bot = 0;	
	demo = 0;	
        
	int in;

	int acctr;

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
	fclose(logfile);

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

		avgaccel = (avgaccel*9 + nunlen())/10;
		
		if (accelZ < 560) acctr++;
		else acctr = 0;	
		if (acctr > 20 && !demo) snakepause();

		if(lastmove+tick_delay <= ctr){
			lastmove = ctr;
			
			
			if (bot) dir = autodir();
			else dir = nextdir;
			mvprintw(13,0,"dir %d", dir);
			refresh();  //ncurses
			
			snake_do(dir);
			
			
			if(bot) dijkstra(pos);
                }
		
		//debug speed control
		/*if(ctr%5==0){
			if(!buttonC && tick_delay > 0) tick_delay--; 
			if(!buttonZ && tick_delay < 30) tick_delay++; 
		}*/
		if(!buttonC){
			loose2();
			snakesetup();
		}
				
		//setpixel((accelX-512)/10+WIDTH/2,(accelY-512)/20+6,127,32,16);

		
		mvprintw(0,2,"pos, nompos: %d, %d\n", pos, nompos);
		mvprintw(1,0,"joy X,Y: %d, %d\n", joyX, joyY);
		mvprintw(2,0,"Accel X,Y,Z: %d, %d, %d\n", accelX, accelY, accelZ);
		mvprintw(1,24,"Accel (avg, cur): %f, %f\n", avgaccel, nunlen());
		mvprintw(3,0,"Button C,Z: %d, %d\n", buttonC, buttonZ);
		mvprintw(10,0,"distance2[nompos], distance[nompos]: %d, %d\n", distance2[nompos], distance[nompos]);
		mvprintw(11,0,"nomreplaced: %d", nomreplaced);
		mvprintw(15,0,"autostatus: %s       ", autostatus);
		calc_score();
		mvprintw(4,5,"Score: %d     ", score);	
		refresh();  //ncurses
		//erase();
		if(abs(avgaccel-200) > 50 && ctr%8 < 4){
			if(!demo){
				int i;
				char text[] = "TILT";
				for (i=0; i<4; i++){
					draw_char(i*7+1+(ctr/10*7)%5,text[i]);
				}
			}
			else{
				loose2();
				snakesetup();
			}
			
		}
		


		//Dijkstra debug
		if(bot){
			int n;
			for(n=0;n<LED_NUM;n++){
				if(distance[n] < 99998 && !isonsnake(n)){
					int tmpv = 8-distance[n]/6;  //(1+sin(distance[n]))*10;
					if (tmpv > 0) buffer[n*3+2] += tmpv;
				}
				else if(distance[n] > 99998 && !isonsnake(n))
					buffer[n*3+2] += (1+sin(ctr/10.0))*15;
				
				//if(deadend[n]) {buffer[n*3+2] = buffer[n*3+1] = buffer[n*3] = 127*((ctr/2)%2);}
				
			}
		}
		
		if (bot) draw_path();
		snake_draw(1);
		effect_draw(ctr);
		buffer[nompos*3+1] = 127;
		
		blit();    
        	update();
		ctr++;
		}


        endwin();                       /* End curses mode                */

        return 0;


}



