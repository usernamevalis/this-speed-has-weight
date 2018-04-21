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


// socket io variabl
var socket;
socket = io.connect();

/* Global variables used by socketio events
 * async socket events update globals, which address
 * then used to update states/variables in the draw loop
 */
var x, y, z, sound = 0;
var ipAddress;
var portNumber;

function setup() {
  // put setup code here
  // need full screen canvas that is responsive to different screen size, especially mobile
  createCanvas(windowWidth, windowHeight);
  frameRate(30);
}

function draw() {
  // put drawing code here
  background(100);
  textSize(28)
  textAlign(CENTER);
  text(ipAddress + ":" + portNumber, windowWidth / 2, 30);


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
    x = data[0]; //rather make sure the values sent are in the correct range, dont process here
    y = data[1];
    z = data[2];
  }
);

function map_range(value, low1, high1, low2, high2) {
  return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}
