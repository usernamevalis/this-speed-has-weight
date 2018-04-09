#!/bin/bash

echo "setting up NPM"
sudo chmod +x install_npm.sh
./install_npm.sh
echo "done."

echo "install installing project dependencies"
npm ../project-template/webserver

echo "setting up the Arduino IDE"
sudo chmod +x install_arduino_ide.sh
./install_arduino_ide.sh
echo "done."

echo "making a backup of the project template, and placing it in 'backups'"
cp ../project-template ../backups/project-template-master
echo "done and done."
echo "goodbye"
