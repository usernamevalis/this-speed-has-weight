#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "updating"
yes Y | sudo apt-get update

echo "installing npm"
yes Y | sudo apt-get install npm
echo "done."

echo "install installing project dependencies"
npm install ./../project-template/webserver/ --prefix ../project-template/webserver/

echo "Downloading Arduino Ide to current folder"
wget https://downloads.arduino.cc/arduino-1.8.5-linuxarm.tar.xz
echo "unzipingn Arduino Archive"
tar -xvf arduino-1.8.5-linuxarm.tar.xz
echo "Installing Arduino IDE"
sudo mv arduino-1.8.5 /opt/ && rm arduino-1.8.5-linuxarm.tar.xz
sudo /opt/arduino-1.8.5/./install.sh
echo "done."

echo "done and done."
echo "goodbye"
