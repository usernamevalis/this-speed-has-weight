#!/bin/bash

echo "Downloading Arduino Ide to current folder"
wget https://downloads.arduino.cc/arduino-1.8.5-linuxarm.tar.xz

echo "unziping"
tar -xvf arduino-1.8.5-linuxarm.tar.xz

echo "installing"
sudo mv arduino-1.8.5 /opt/ && rm arduino-1.8.5-linuxarm.tar.xz
/opt/arduino-1.8.5/./install.sh
