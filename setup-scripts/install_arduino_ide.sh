#!/bin/bash

echo "Downloading Arduino Ide to current folder"
wget https://downloads.arduino.cc/arduino-arduino-1.8.5-linuxarm.tar.xz

echo "unziping"
unzip arduino-1.8.5-linuxarm.tar.xz

echo "installing"
cd arduino-1.8.5
./install.sh
