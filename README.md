# this-speed-has-weight
project repo for the Workshop in Plataforma Bogota April/March 2018
<br>

Basic nodejs server communicates with p5js frontend over websockets. <br>
The server collects sensor information from an Arduino on the serial port and updates the p5js frontend using that information.<br>

<b>Instructions:</b>

This following instructions will install the Arduino IDE, the NPM package manager and setup the template project on your Rpasberyy Pi.
Dont forget to change your timezone and enable SSH if you need it. You can do this by running the following cmd in the terminal:

`sudo raspi-config`

NB This script enables ssh so you must change your default password using the
`passwd`
command in the terminal

TO begin make sure you are connected to the internet. Then open a terminal and copy and paste the following lines of code, one by one.

1. Download the repository with the following terminal cmd:

    `cd ~/ && wget https://github.com/usernamevalis/this-speed-has-weight/archive/master.zip`

2. Unzip the downloaded folder and make the install script executable:

    `unzip master.zip && chmod +x this-speed-has-weight-master/setup-scripts/install.sh`

3. Run the install script inside of the 'setup-scripts' folder

    `this-speed-has-weight-master/setup-scripts/./install.sh`

 Here are all the commands in one line that you can copy and paste

    cd ~/ && wget https://github.com/usernamevalis/this-speed-has-weight/archive/master.zip && unzip master.zip && chmod +x this-speed-has-weight-master/setup-scripts/install.sh && this-speed-has-weight-master/setup-scripts/./install.sh
