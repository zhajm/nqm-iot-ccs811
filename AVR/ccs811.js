const Serialport = require('serialport');
const serialport = Serialport.SerialPort; 
const readline = require('readline'); 
const filesystem = require('fs');

var port = new Serialport('/dev/ttyUSB0', {
	baudrate: 38400,
	parser: Serialport.parsers.readline('\n')
});

var rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout});

var index = 0;
var log;

port.on('open', function() {
	log = filesystem.createWriteStream(
			'/home/pi/Desktop/AVR/testnew.txt',
			{flags: 'w'});
	console.log("Serial Port Opened" + "\n");
});

port.on('data', function(data) {
	//console.log(data);
	onDataReceived.call(this, data);
});

var onDataReceived = function(data) {
	switch (data[0]) {
		case "C":
			var CO2data= ("Time:	" + Date.now() + '	CO2:	' + data.substring(3));
			CO2data = CO2data.replace("\r\r",""); 
			log.write(CO2data + "\n");
			console.log(CO2data);
			break;
		case "V":
			var TVOC = ("Time:	" + Date.now() + "	TVOC:	" + data.substring(3) + "\n");
			console.log(TVOC);
			break;
		case "H":
			var RH = ("Time:	" + Date.now() + "	RH:	" + data.substring(3));
			console.log(RH);
			break;
		case "T":
			var TEMP = ("Time:	" + Date.now() + "	Temp:	" + data.substring(3));
			console.log(TEMP);
			break;   
	//	case "I":
	//		console.log(data + "\n");
	//		break;
		default:
			console.log("ignoring data: " + data + "\n");
			break;
	}
};

//var poll = function() {
//  setTimeout(function() {
//	    port.write("Z\r\n"); 
//  }, 500);
//setTimeout(function() {
//	    port.write("T\r\n"); 
//  }, 1000);
//setTimeout(function() {
//	    port.write("H\r\n"); 
//  }, 1500);
//};

//rl.on('line', function(line) {
//	console.log('Manual Input!')
//	//var chmod = 'K 2\r\n';
//
//	//setInterval(function(){port.write(chmod)}, 5000);

//	//port.write(line + '\r\n');

//});
