/*
 * NodeJS Socketio Server for Plataforma Workshop : This speed has weight
 * This app connects to a Arduino MCU over the usb/serial connection.
 * You can read informatino from the arduino as well as write information to it.
 * How exactly you do this depends on how you ahve set up your arduino sketch
 *
 * This NodeJS app also serves a P5js webpage and communicates with it using SocketIO (websockets)
 * This allows you to update your p5js app/website in near real time using information from your arduino
 * and vice versa
 *
 * Install project dependecies by running 'npm install' inside of the project folder, from a terminal
 * This app is asynchronous to a certain degree so bear in mind when troubleshooting that not all is as it seems
 * everything is event driven.
 *
 * This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * Nathan Gates 2018
 */


var util = require('util'); //console.log(util.inspect(myObject, false, null));
var express = require('express');
var app = express();
var path = require('path');
var os = process.platform;
var portname;
var contact = false;

if (os == "linux") {
  portname = "/dev/ttyACM0"
} else {
  portname = 'COM11';
}

//=========================Static File Webserver=================//
app.use(express.static(__dirname + '/public'));

//route examples
app.route('/test')
  .get(function(req, res) {
    res.sendFile(path.resolve('public/html/test.html'));
  });

//404 Response
app.get('*', function(req, res, next) {
  res.status(404);
  res.sendFile(path.resolve('public/html/404.html'));
});

var server = app.listen(3000, '0.0.0.0', function() {
  console.log('Server is happenning on localIP: %s:%s', addresses, server.address().port);
});

//======================Serial=================//
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;

var myPort = new SerialPort(portname, {
  baudRate: 115200
});
const parser = new Readline();
myPort.pipe(parser);

myPort.on("open", function() {
  console.log('open');
  //pollArduino(2000, 250);
  parser.on('data', function(data) {
    var dataPacket = data.split(',');
    console.log(dataPacket[0]);
    console.log(Boolean(contact));
    if (Boolean(contact) == true) {
      myPort.write("r\r");
      for (var i = 0; i < dataPacket.length; i++) {
        console.log(dataPacket[i]);
      }
      io.sockets.emit('sensorData', dataPacket);
      myPort.write("r\r");
    } else {
      myPort.write('r\r');
      contact = true;
    }
  });
});

// Error Handlers
myPort.on('error', function(err) {
  console.log("ERROR ERROR: " + err);
});

myPort.on('close', function() {
  console.log("port is now closed");
});

//=========================SocketIO====================//
var io = require('socket.io').listen(server);

//Connection event
io.sockets.on('connection', function(socket) {
  console.log('a user connected');

  //send data to single user - this will happen as soon as they connect
  var ipInfo = [
    addresses,
    server.address().port
  ]
  socket.emit('IP', ipInfo);

  //Disconnection event
  socket.on('disconnect', function() {
    console.log('a user disconnected');
  });

  //Error Handlers
  socket.on('error', function(err) {
    console.error('Network connection error', err);
  });

});

//=========================Funcitons=================//

function pollArduino(initialDelay, pollingDelay) {
  var receive = setInterval(function() {
    myPort.write("r\r");
    clearInterval(receive);
    receive = setInterval(function() {
      myPort.write("r\r");
    }, pollingDelay);
  }, initialDelay);
}

function initArduino(initialDelay) {
  var receive = setInterval(function() {
    myPort.write("r\r");
    clearInterval(receive);
  }, initialDelay);
}

//=========================Utilities=================//
//Get Local Ip Address
var os = require('os');
var interfaces = os.networkInterfaces();
var addresses = [];
for (var k in interfaces) {
  for (var k2 in interfaces[k]) {
    var address = interfaces[k][k2];
    if (address.family === 'IPv4' && !address.internal) {
      addresses.push(address.address);
    }
  }
}
