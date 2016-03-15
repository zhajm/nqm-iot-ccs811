
module.exports = (function() {
  "use strict";

  var console = { log: require("debug")("iotHub") };
  var config = require("./config.js");

  var cacheLib = require("nqm-iot-file-cache");
  var cache = new cacheLib.FileCache(config.getLocal("cacheConfig",{}));

  var syncConfig = config.getLocal("syncConfig",{ syncType: "nqm-iot-http-sync" });
  var syncLib = require(syncConfig.syncType);
  var sync = new syncLib.Sync(syncConfig);
  sync.initialise(function(err, reconnect) {
    if (!err) {
      cache.setSyncHandler(sync);
    } else {
      console.log("failed to initialise sync: " + err.message);
    }
  });

  var handleDriverData = function(feedId, data) {
    var feedData = {
      id: feedId,           // Feed id
      d: data               // Payload data.
    };
    cache.cacheThis(feedData);
  };

  // For each driver in config.
  var driversConfig = config.getLocal("drivers",[]);
  for (var d in driversConfig) {
    var driverConfig = driversConfig[d];
    console.log("loading driver " + driverConfig.type);
    // Load driver module.
    var moduleName = "nqm-iot-" + driverConfig.type.toLowerCase() + "-driver";
    try {
      var driverLib = require(moduleName);

      // Init with config
      var driverObj = new driverLib.Driver(driverConfig);

      // Subscribe to "data" event
      driverObj.on("data", handleDriverData);
      driverObj.start();
    } catch (e) {
      console.log("failed to load driver '" + moduleName + "': " + e.message);
    }
  }

  return {};
}());
