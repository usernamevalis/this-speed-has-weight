# this-speed-has-weight
project repo for the Workshop in Plataforma Bogota April/March 2018
<br>

Basic nodejs server communicates with p5js frontend over websockets. <br>
The server collects sensor information from an Arduino on the serial port and updates the p5js frontend using that information.<br>

The Arduio serial protocol is straight forward and easy add to more sensors/information. I will add descriptions shortly<br>

Still to do:<br>
Add boot scripts for RPI<br>
Add install / setup scripts for nodejs for RPI<br>
Add Arduino code<br>
Extend socket API<br>
Develop p5js template<br>


Instructions:

Make sure you are connected to the internet

1. Download the repository with the following terminal cmd:

      wget https://github.com/usernamevalis/this-speed-has-weight/archive/master.zip

2. Unzip the downloaded folder and remove the archive:

      unzip master.zip

3. Make the install file executable

      chmod +x this-speed-has-weight-master/setup-scripts/install.sh

4. Run the install script inside of the 'setup-scripts' folder

      this-speed-has-weight-master/setup-scripts/./install.sh
