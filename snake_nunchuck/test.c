

#include <ncurses.h>




int main(void){
	
	int in;

	initscr();			/* Start curses mode 		  */
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	cbreak();
	noecho();


	printw("Hello World !!!");	/* Print Hello World		  */
	refresh();			/* Print it on to the real screen */
	



	while(42){
		in = getch();
		if (in == KEY_LEFT){
			printw("left!");			/* Wait for user input */
			refresh();	
		}
	}


	endwin();			/* End curses mode		  */

	return 0;





}
