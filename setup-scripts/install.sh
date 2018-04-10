#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "updating"
yes Y | sudo apt-get update

echo "installing npm"
yes Y | sudo apt-get install npm
echo "done."

echo "install installing project dependencies"
npm --prefix "$DIR/../project-template/webserver/" install  "$DIR/../project-template/webserver/"

echo "Downloading Arduino Ide to current folder"
wget https://downloads.arduino.cc/arduino-1.8.5-linuxarm.tar.xz
echo "unzipping Arduino Archive"
tar -xvf arduino-1.8.5-linuxarm.tar.xz
echo "Installing Arduino IDE"
sudo mv arduino-1.8.5 /opt/ && rm arduino-1.8.5-linuxarm.tar.xz
sudo /opt/arduino-1.8.5/./install.sh
echo "done."

echo "Backing up project template into 'backups' folder"
cp -r "$DIR/../project-template" "$DIR/../backups/"

echo "Moving master archive to backups"
mv "$DIR/../../master.zip"  "$DIR/../backups/" 

echo "done and done."
echo "goodbye"
