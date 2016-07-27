"use strict";

module.exports = (function() {
  var console = { log: require("debug")("COZIRDriver") }
  var util = require("util");
  var serialModule = require("serialport");
  var delimiter = "\r\n";
  var eventEmitter = require('events').EventEmitter;

  function cozir(config) {
    eventEmitter.call(this);

    this._config = config;
    this._serialPort = null;
    this._timer = 0;
    this._co2 = 0;
    this._temperature = 0;
    this._humidity = 0;
  }

  util.inherits(cozir, eventEmitter);

  cozir.prototype.start = function() {
    var self = this;

    this._serialPort = new serialModule.SerialPort(this._config.port, { parser: serialModule.parsers.readline(delimiter), baudrate: 9600}, false);

    this._serialPort.open(function(err) {
      if (typeof err !== "undefined" && err !== null) {
        console.log("cozir - failed to open port " + self._config.port + " - " + JSON.stringify(err));
      } else {
        console.log("cozir - opened port");

        self._serialPort.on("error", function(e) {
          console.log("cozir - port error: " + JSON.stringify(e));
        });

        self._serialPort.on("data", function (data) {
          if (typeof data !== "undefined" && data !== null) {
            console.log("cozir: " + data);
            onDataReceived.call(self, data);
          }
        });

        // Request configuration (sometimes required to get unit to listen to operating mode request.
        setTimeout(function() { self._serialPort.write("*\r\n"); }, 1000);

        // Set 'poll' operating mode.
        setTimeout(function() { self._serialPort.write("K 2\r\n"); }, 5000);
      }
    });
  };

  cozir.prototype.stop = function() {
    if (this._serialPort !== null) {
      this._serialPort.close();
      this._serialPort = null;
    }
  };

  var startPolling = function() {
    if (this._timer === 0) {
      this._timer = setInterval(poll.bind(this), this._config.cozirPollInterval*60*1000);
    }
  };

  var poll = function() {
    var self = this;

    // ToDo - review sequencing.
    setTimeout(function() { self._serialPort.write("Z\r\n"); }, 500);
    setTimeout(function() { self._serialPort.write("T\r\n"); }, 5500);
    setTimeout(function() { self._serialPort.write("H\r\n"); }, 10500);
  };

  var handleCO2 = function(data) {
    var co2 = parseInt(data.substr(2));
    if (co2 !== this._co2) {
      this._co2 = co2;
      this.emit("data", this._config.feedId, { timestamp: Date.now(), co2: co2 });
    }
  };

  var handleHumidity = function(data) {
    var humidity = parseInt(data.substr(2))/10;
    if (humidity !== this._humidity) {
      this._humidity = humidity;
      this.emit("data", this._config.feedId, { timestamp: Date.now(), humidity: humidity });
    }
  };

  var handleTemperature = function(data) {
    var temp = (parseInt(data.substr(2)) - 1000) / 10;
    if (temp !== this._temperature) {
      this._temperature = temp;
      this.emit("data", this._config.feedId, { timestamp: Date.now(), temperature: temp});
    }
  };

  var onDataReceived = function(data) {
    switch (data[1]) {
      case "Z":
        handleCO2.call(this,data);
        break;
      case "T":
        handleTemperature.call(this,data);
        break;
      case "H":
        handleHumidity.call(this,data);
        break;
      case "K":
        startPolling.call(this);
        break;
      default:
        console.log("ignoring data: " + data);
        break;
    }
  };

  return cozir;
}());
