
#include <stdio.h>

#include "../rgblib.h"
#include "../screenlib.h"

#include "menu.h"


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
