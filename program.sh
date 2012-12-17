#!/bin/bash

./blinken29 &
sleep 7
killall blinken29
./blinkenflash &
sleep 13
killall blinkenflash
./program.sh
