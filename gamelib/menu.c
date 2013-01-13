
#include <stdio.h>
#include <stdlib.h>

#include "../rgblib.h"
#include "../screenlib.h"

#include "menu.h"


struct menu* menu_create(){
	
	struct menu* new = (struct menu *)malloc(sizeof(struct menu));

	new->selected = 0;	
	new->length = 0;	
	new->animation = 0.0;
	new->entries = (struct menu_entry *)malloc(sizeof(struct menu_entry)*MENU_MAXSIZE);	
}

struct menu_entry* menu_entry(struct menu* m){
	if (m->length >= MENU_MAXSIZE) return;	
	m->length++;
	
	return &(m->entries[m->length-1]);
}


void menu_show(struce menu* ){
	
	
	
	
	
	
	
}


int main(void){
	double ts = 0.0;


	spi_init();

	

	while (draw_string(ts, "teststring") <= 1.0){
		printf("ts: %f\n", ts);	
		
		blit();
		update();
		
		ts += 0.1;
	}

}
