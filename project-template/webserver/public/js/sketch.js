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
var _data = [];
var data = {
  x_orientation: 0,
  y_orientation: 0,
  z_orientation: 0,
  temperature: 0,
  pressure: 0,
  altitude: 0,
  humidity: 0,
  irLight: 0,
  lux: 0,
  lat: 0,
  long: 0,
  altGps: 0,
  speedKm: 0,
  sats: 0,
  button1:0,
  button2:0,
  button3:0,
  button4:0,
  button5:0
};

var ipData = {
  ipAddress: 0,
  portNumber: 0
};

var updating = true;

function setup() {
  // put setup code here
  // need full screen canvas that is responsive to different screen size, especially mobile
  createCanvas(windowWidth, windowHeight);
  frameRate(30);
}

//rawest possible data as feasible - need to adjust per project as needed
//soundLevel : 0.0 -5.0;
//x,y,z : 0/360 , -180/180, -180/180;
function draw() {

  fill(255);
  background(0);

  //example use of data object
  //background(data.lux);

  //display ip address on screen
  displayIP();

  //update sensor data object
  updateSensorData();
  //
  defaultDisplay();

  push();
  rotateDisplay(data.x_orientation);
  pop();
}

function rotateDisplay(degrees) {
  translate((windowWidth / 2) - 50, (windowHeight / 2) - 50);
  rotate(radians(degrees));
  rectMode(CENTER)
  fill(255);
  rect(0, 0, 100, 100);
}

//====================Socket IO Events / messages==============//
/* You cant use p5js funcitons inside of socket callbacks
 *  rather do any processing on server side / arduino inside
 *  and just send ready to use info in the websocket.
 *  Alternatively, process them in the draw loop
 */
socket.on('IP',
  function(data) {
    var counter = 0;
    for (var i = 0; i < data.length; i++) {
      ipData.ipAddress = data[0];
      ipData.portNumber = data[1];
    }
  }
);

socket.on('sensorData',
  function(data) {
    updating = true;
    for (var i = 0; i < data.length; i++) {
      _data[i] = data[i];
    }
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
  text(ipData.ipAddress + ":" + ipData.portNumber, windowWidth / 2, 30);
}

function updateSensorData() {
  if (!updating) {
    var counter = 0;
    for (var key in data) {
      if (data.hasOwnProperty(key)) {
        data[key] = _data[counter];
        counter++
      }
    }
    // for (var key in data) {
    //   if (data.hasOwnProperty(key)) {
    //     console.log(key + " -> " + data[key]);
    //   }
    // }
  }
}

function defaultDisplay() {
  //display data
  textSize(26);
  var x = 50
  var y = 50;
  var yInc = windowHeight / ((Object.keys(data).length) / 1.5);
  var xInc = windowWidth / 2;
  var counter = 0;

  for (var key in data) {
    textAlign(LEFT);
    if (data.hasOwnProperty(key)) {
      var val = data[key];
      if (counter <= int((Object.keys(data).length) / 2)) {
        text(key + ' : ' + val, x, y += yInc);
        counter++;
      } else {

        text(key + ' : ' + val, x + xInc, y);
        y -= yInc;
      }
    }
  }
}

// when the window is resized the canvas is resized accordingly
function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}
