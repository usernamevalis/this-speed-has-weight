var util = require('util'); //console.log(util.inspect(myObject, false, null));
var express = require('express');
var app = express();
var path = require('path');
var portname = 'COM4';

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

var server = app.listen(3000, function() {
  console.log('Server is happenning on localIP: %s:%s', addresses, server.address().port);
});

//======================Serial=================//
// const SerialPort = require('serialport');
// const Readline = SerialPort.parsers.Readline;
//
// var myPort = new SerialPort(portname, {
//   baudRate: 115200
// });
// const parser = new Readline();
// myPort.pipe(parser);
//
// myPort.on("open", function() {
//   console.log('open');
//
//   /*sending "read\r" down the serialport makes the
//   arduino read all its sensors - stil to be defined
//   and send that information back over the serialport
//   formatted to be parsed by the serialparser */
//   var receive = setInterval(function() {
//     myPort.write("read1\r");
//     clearInterval(receive);
//     receive = setInterval(function() {
//       myPort.write("read1\r");
//     }, 100);
//   }, 2000);
//
//   parser.on('data', function(data) {
//     var dataPacket = data.split(',');
//     var lightData = dataPacket[0];
//     io.emit("sensor1", lightData);
//   });
// });
//
// myPort.on('error', function(err) {
//   console.log("ERROR ERROR: " + err);
// });
//
// myPort.on('close', function() {
//   console.log("port is now closed");
// });

//=========================SocketIO=================//
var io = require('socket.io').listen(server);
io.sockets.on('connection', function(socket) {
  console.log('a user connected');
  socket.emit('pingTest', 'ping');

  var fakeData = 0;
  var fakeDataInterval = setInterval(function() {
    if (fakeData < 255) {
      fakeData += 10;
      socket.emit('sensorData1', fakeData);
    } else {
      fakeData = 0;
    }
  }, 50);

  socket.on('disconnect', function() {
    console.log('a user disconnected');
  });

  socket.on('error', function(err) {
    console.error('Network connection error', err);
  });

});

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
