// socket io variable
var socket;
socket = io.connect();

/* Global variables used by socketio events
 * async socket events update globals, which address
 * then used to update states/variables in the draw loop
 */
var sensorData1 = 0;

function setup() {
  // put setup code here
  // need full screen canvas that is responsive to different screen size, especially mobile
  createCanvas(displayWidth, displayHeight);
}

function draw() {
  // put drawing code here
  background(sensorData1);
}

socket.on('pingTest',
  function(data) {
    console.log("message from server:" + data);
  }
);
socket.on('sensorData1',
  function(data) {
    sensorData1 = data; //can constrain or limit data range here
    console.log(data);
  }
);
