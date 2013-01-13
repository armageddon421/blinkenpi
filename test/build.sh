

cc -o blinkenpi main.c ../rgblib.c ../screenlib.c -I/usr/local/include -L/usr/local/lib -lwiringPi -lm -lncurses
