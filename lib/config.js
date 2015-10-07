"use strict";

module.exports = (function() {
  var console = { log: require("debug")("config") }
  var fs = require("fs");
  var path = require("path");
  var config = require("../config.json");
  var localConfigPath = path.join(__dirname,"../","config.local.json");
  var localConfig = {};

  var loadLocal = function() {
    if (fs.existsSync(localConfigPath)) {
      var txt = fs.readFileSync(localConfigPath);
      try {
        localConfig = JSON.parse(txt);
      } catch (e) {
        console.error("failed to parse config file - aborting...",e);
        process.exit();
      }
    } else {
      console.log("config file missing - creating new");
      localConfig = {};
    }
    return localConfig;
  };

  var saveLocal = function() {
    fs.writeFileSync(localConfigPath, JSON.stringify(localConfig,null,2));
  };

  var getLocal = function(name, def) {
    loadLocal();
    if (typeof name === "undefined") {
      return localConfig;
    } else {
      if (!localConfig.hasOwnProperty(name)) {
        localConfig[name] = def;
      }

      return localConfig[name];
    }
  };

  var setLocal = function(name,val) {
    localConfig[name] = val;
    saveLocal();
  };

  var resetLocal = function() {
    localConfig.sessionTransmit = 0;
    localConfig.totalTransmit = 0;
    localConfig.seenDevices = {};
    delete localConfig.devKey;
    delete localConfig.name;

    saveLocal();
  };

  loadLocal();

  return {
    get: function() { return config; },
    getLocal: getLocal,
    setLocal: setLocal,
    resetLocal: resetLocal
  };
}());

