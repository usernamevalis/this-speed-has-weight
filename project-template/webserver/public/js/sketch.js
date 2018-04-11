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
var sensorData0 = 0;
var sensorData1 = 0;
var sensorData2 = 0;

function setup() {
  // put setup code here
  // need full screen canvas that is responsive to different screen size, especially mobile
  createCanvas(displayWidth, displayHeight);
}

function draw() {
  // put drawing code here
  background(sensorData0, sensorData1, sensorData2);
}

//====================Socket IO Events / messages==============//
/* You cant use p5js funcitons inside of socket callbacks
 *  rather do any processing on server side / arduino inside
 *  and just send ready to use info in the websocket.
 *  Alternatively, process them in the draw loop
 */

socket.on('pingTest',
  function(data) {
    console.log("message from server:" + data);
  }
);

socket.on('sensorData',
  function(data) {
    sensorData0 = data[0] / 4; //rather make sure the values sent are in the correct range, dont process here
    sensorData1 = data[1] / 4;
    sensorData2 = data[2] / 4;
  }
);
