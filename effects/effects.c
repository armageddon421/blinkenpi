
#include <stdio.h>

#include "../rgblib.h"
#include "../screenlib.h"

#include "effects.h"

//private prototypes
void effect_draw_delay(int time, int ef);
void effect_draw_strobe(int time, int ef);
void effect_draw_fadein(int time, int ef);
void effect_draw_fadeout(int time, int ef);
void effect_draw_explosion_plus(int time, int ef);
void effect_draw_explosion_x(int time, int ef);


extern unsigned char buffer[LED_NUM];

struct effect eflist[EFFECT_BUFSIZE];


void effect_init(){


}


int effect_create(int type, int pos, int red, int green, int blue, int parameter, int time, int duration){
	int i;
	for(i=0;i<EFFECT_BUFSIZE;i++){
		if (!eflist[i].active){
			eflist[i].active = 1;
			eflist[i].type = type;
			eflist[i].pos = pos;
			eflist[i].red = red;
			eflist[i].green = green;
			eflist[i].blue = blue;
			eflist[i].parameter = parameter;
			eflist[i].starttime = time;
			eflist[i].duration = duration;
			return 0;
		}
	}
	return 1;
}

void effect_reset(){
	int i;
	for (i=0;i<EFFECT_BUFSIZE;i++){
		eflist[i].active = 0;
	}

}

int effect_draw(int time){
	int i;
	for (i=0;i<EFFECT_BUFSIZE;i++){
		if(eflist[i].active){
			switch(eflist[i].type){
				case(EFFECT_TYPE_DELAY):
					effect_draw_delay(time, i);
					break;
				case(EFFECT_TYPE_STROBE):
					effect_draw_strobe(time, i);
					break;
				case(EFFECT_TYPE_FADEIN):
					effect_draw_fadein(time, i);
					break;
				case(EFFECT_TYPE_FADEOUT):
					effect_draw_fadeout(time, i);
					break;
				
				case(EFFECT_TYPE_EXPLOSION):
					effect_draw_explosion_plus(time, i);
					effect_draw_explosion_x(time, i);
					break;
				case(EFFECT_TYPE_EXPLOSION_PLUS):
					effect_draw_explosion_plus(time, i);
					break;
				case(EFFECT_TYPE_EXPLOSION_X):
					effect_draw_explosion_x(time, i);
					break;
				
				
			}
		}
	}
	
}

void effect_draw_delay(int time, int ef){

	if( time >= eflist[ef].starttime && time < eflist[ef].starttime + eflist[ef].duration){
		if(eflist[ef].pos < 0 || eflist[ef].pos >= LED_NUM){
			eflist[ef].active = 0;
			return;
		}
		
		buffer[eflist[ef].pos*3 + 0] += eflist[ef].red;	
		buffer[eflist[ef].pos*3 + 1] += eflist[ef].green;	
		buffer[eflist[ef].pos*3 + 2] += eflist[ef].blue;	
		
	}
	else if (time >= eflist[ef].starttime + eflist[ef].duration){
		eflist[ef].active = 0;
	}
}

void effect_draw_strobe(int time, int ef){
	
	if( time >= eflist[ef].starttime && time < eflist[ef].starttime + eflist[ef].duration){
		if(eflist[ef].pos < 0 || eflist[ef].pos >= LED_NUM){
			eflist[ef].active = 0;
			return;
		}
		if ((time-eflist[ef].starttime)%(eflist[ef].parameter*2) < eflist[ef].parameter){	
			buffer[eflist[ef].pos*3 + 0] += eflist[ef].red;	
			buffer[eflist[ef].pos*3 + 1] += eflist[ef].green;	
			buffer[eflist[ef].pos*3 + 2] += eflist[ef].blue;	
		}	
	}
	else if (time >= eflist[ef].starttime + eflist[ef].duration){
		eflist[ef].active = 0;
	}
}

void effect_draw_fadein(int time, int ef){

	if( time >= eflist[ef].starttime && time < eflist[ef].starttime + eflist[ef].duration){
		if(eflist[ef].pos < 0 || eflist[ef].pos >= LED_NUM){
			eflist[ef].active = 0;
			return;
		}
		double f = (time-eflist[ef].starttime)/(double)eflist[ef].duration;	
		buffer[eflist[ef].pos*3 + 0] += eflist[ef].red*f;	
		buffer[eflist[ef].pos*3 + 1] += eflist[ef].green*f;	
		buffer[eflist[ef].pos*3 + 2] += eflist[ef].blue*f;	
		
	}
	else if (time >= eflist[ef].starttime + eflist[ef].duration){
		eflist[ef].active = 0;
	}
}

void effect_draw_fadeout(int time, int ef){

	if( time >= eflist[ef].starttime && time < eflist[ef].starttime + eflist[ef].duration){
		if(eflist[ef].pos < 0 || eflist[ef].pos >= LED_NUM){
			eflist[ef].active = 0;
			return;
		}
		double f = 1.0 - (time-eflist[ef].starttime)/(double)eflist[ef].duration;	
		buffer[eflist[ef].pos*3 + 0] += eflist[ef].red*f;	
		buffer[eflist[ef].pos*3 + 1] += eflist[ef].green*f;	
		buffer[eflist[ef].pos*3 + 2] += eflist[ef].blue*f;	
		
	}
	else if (time >= eflist[ef].starttime + eflist[ef].duration){
		eflist[ef].active = 0;
	}
}


void effect_draw_explosion_plus(int time, int ef){
	struct effect e = eflist[ef];
	
	if( time == e.starttime){
		int i;
		int iter_duration = e.duration/(e.parameter+1);
		for (i=1;i<e.parameter+1;i++){
			effect_create(EFFECT_TYPE_STROBE, e.pos+i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos+i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos+i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 
			
			effect_create(EFFECT_TYPE_STROBE, e.pos-i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos-i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos-i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 

			effect_create(EFFECT_TYPE_STROBE, e.pos+WIDTH*i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos+WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos+WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 
			
			effect_create(EFFECT_TYPE_STROBE, e.pos-WIDTH*i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos-WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos-WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 
		}
	}
	if( time > e.starttime){
		eflist[ef].active = 0;
	}

}

void effect_draw_explosion_x(int time, int ef){
	struct effect e = eflist[ef];
	
	if( time == e.starttime){
		int i;
		int iter_duration = e.duration/(e.parameter+1);
		for (i=1;i<e.parameter+1;i++){
			effect_create(EFFECT_TYPE_STROBE, e.pos+i+WIDTH*i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos+i+WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos+i+WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 
			
			effect_create(EFFECT_TYPE_STROBE, e.pos-i+WIDTH*i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos-i+WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos-i+WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 

			effect_create(EFFECT_TYPE_STROBE, e.pos-i-WIDTH*i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos-i-WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos-i-WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 
			
			effect_create(EFFECT_TYPE_STROBE, e.pos+i-WIDTH*i, e.red, e.green, e.blue, 1, time+iter_duration*(i-1), iter_duration*2); 
			effect_create(EFFECT_TYPE_FADEOUT, e.pos+i-WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i-1), iter_duration); 
			effect_create(EFFECT_TYPE_FADEIN, e.pos+i-WIDTH*i, -e.red, -e.green, -e.blue, 0, time+iter_duration*(i), iter_duration); 
		}
	}
	if( time > e.starttime){
		eflist[ef].active = 0;
	}

}







