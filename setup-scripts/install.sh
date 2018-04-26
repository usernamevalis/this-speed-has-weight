#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "updating"
yes Y | sudo apt-get update

echo "enabling SSH"
sudo touch /boot/ssh

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

echo "copying across arduino libraries"
cp -r "$DIR/../project-template/arduino/libraries/" "/home/pi/Arduino/libraries/"

echo "Backing up project template into 'backups' folder"
cp -r "$DIR/../project-template" "$DIR/../backups/"

echo "Moving master archive to backups"
mv "$DIR/../../master.zip"  "$DIR/../backups/"

echo "setting up chromium autostart"
FILE="/home/pi/.config/lxsession/LXDE-pi/autostart"

/bin/cat <<EOM >$FILE
@lxpanel --profile LXDE-pi
@pcmanfm --desktop --profile LXDE-pi
@xscreensaver -no-splash
@point-rpi
@xset s noblank
@xset s off
@xset –dpms
@chromium-browser --incognito http://localhost:3000/
EOM

echo "setting up nodejs auto start"
echo "installing forever"
sudo npm install forever -g

#clear crontab to prevent mulitple entries
echo "setting up cron @reboot"
#write out current crontab
crontab -r
crontab -l > mycron
#echo new cron into cron file
echo "@reboot /usr/local/bin/forever start /home/pi/this-speed-has-weight-master/project-template/webserver/app.js" >> mycron
#install new cron file
crontab mycron
rm mycron

# #install lcd driver
# echo "downloading LCD driver"
# wget https://www.waveshare.com/w/upload/3/34/LCD-show-180331.tar.gz -P /boot/
# #setup lcd driver and touch screen
# echo "installing LCD driver"
# tar -xzvf /boot/LCD-show-*.tar.gz -C /boot/
# chmod +x /boot/LCD-show/LCD5-show
# /boot/LCD-show/./LCD5-show

echo "done and done."
echo "goodbye"
