/*
 * This Speed has Weight
 * Plataforma Bogota
 *
 * Simple p5js app that receives data over websockets from a nodejs webserver.
 * The data received is used to change the background colour
 *
 * This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * Nathan Gates 2018
 */


// socket io variable
var socket;
socket = io.connect();

/* Global variables used by socketio events
 * async socket events update globals, which address
 * then used to update states/variables in the draw loop
 */
var _xo, _yo, _zo, _soundLevel = 0;
var x_orientation, y_orientation, z_orientation = 0;
var soundLevel = 0;
var ipAddress;
var portNumber;
var updating = true;

function setup() {
  // put setup code here
  // need full screen canvas that is responsive to different screen size, especially mobile
  createCanvas(windowWidth, windowHeight);
  frameRate(30);
}

//rawest possible data as feasible - need to adjust per project as needed
//soundLevel : 0.0 -5.0;
//x,y,z : 0-360;
function draw() {
  fill(255);
  background(100);
  //display ip address on screen
  displayIP();
  updateSensorData();

  x_orientation = map(x_orientation, 0, 360, 0, 255);
  y_orientation = map(y_orientation, 0, 360, 0, 255);
  z_orientation = map(z_orientation, 0, 360, 0, 255);

  // An ellipse
  fill(x_orientation, y_orientation, z_orientation);
  ellipse(windowWidth / 2, windowHeight / 2, soundLevel * 100, soundLevel * 100);

}

//====================Socket IO Events / messages==============//
/* You cant use p5js funcitons inside of socket callbacks
 *  rather do any processing on server side / arduino inside
 *  and just send ready to use info in the websocket.
 *  Alternatively, process them in the draw loop
 */

socket.on('IP',
  function(data) {
    //console.log("message from server:" + data);
    ipAddress = data[0];
    portNumber = data[1];
  }
);


socket.on('sensorData',
  function(data) {
    updating = true;
    _xo = data[0]; //rather make sure the values sent are in the correct range, dont process here
    _yo = data[1];
    _zo = data[2];
    _soundLevel = data[3];
    updating = false;
  }
);

/*
 *  Functions
 */

function map_range(value, low1, high1, low2, high2) {
  return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}

function displayIP() {
  textSize(28)
  textAlign(CENTER);
  text(ipAddress + ":" + portNumber, windowWidth / 2, 30);
}

function updateSensorData() {
  if (!updating) {
    x_orientation = _xo;
    y_orientation = _yo;
    z_orientation = _zo;
    soundLevel = _soundLevel;
  }
}
