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
var _data = [_xo, _yo, _zo, _soundLevel, _temp, _pres, _alti, _humi, _ir, _vis, _full, _lux];
var data = {
  x_orientation: 0,
  y_orientation: 0,
  z_orientation: 0,
  temperature: 0,
  pressure: 0,
  altitude: 0,
  humidity: 0,
  irLight: 0,
  visibleLight: 0,
  fullLight: 0,
  lux: 0
};
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
    for (var i = 0; i < data.length; i++) {
      _data[0] = data[0];
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
  text(ipAddress + ":" + portNumber, windowWidth / 2, 30);
}

function updateSensorData() {
  if (!updating) {
    console.log(starting);
    for (var key in data) {
      console.log(key);
    }

    // x_orientation = _xo;
    // y_orientation = _yo;
    // z_orientation = _zo;
    // soundLevel = _soundLevel;
    // temperature = _temp;
    // pressure = _pres;
    // altitude = _ alti;
    // humidity = _humi;
    // irLight = _ir;
    // visibleLight = _vis;
    // fullLight = _full;
    // lux = _lux;
  }
}
