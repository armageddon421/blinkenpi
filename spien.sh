#!/bin/bash



sudo modprobe spi_bcm2708
sudo chown `id -u`.`id -g` /dev/spidev0.*
