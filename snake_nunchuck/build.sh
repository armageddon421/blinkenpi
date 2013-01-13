

cc -O2 -o blinkenpi main.c ../rgblib.c ../screenlib.c ../../nunchuck/test.c ../effects/effects.c -I/usr/local/include -L/usr/local/lib -lwiringPi -lm -lncurses
