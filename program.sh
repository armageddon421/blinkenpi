#!/bin/bash

./blinken29 &
sleep 7
killall blinken29
./blinkenpi &
sleep 13
killall blinkenpi
./program.sh
