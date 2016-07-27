"use strict";

exports.httpSync = function(test) {
  var HTTPSync = require("../index.js").Sync;
  var config = {
    "syncServer": "localhost",
    "syncPort": 3102,
    "ssl": false,
    "path": "/feedData"
  };
  var data = {
    hubId: "abcd",
    feedId: "toby1",
    temperature: 32.1
  };


  var sync = new HTTPSync(config);

  sync.initialise(function(err) {
    if (!err) {
      sync.sendData(JSON.stringify(data), function(err, result) {
        test.expect(2);
        test.ok(!err,"completed without errors");
        test.ok(result && result.ok === true, "sync succeeded");
        test.done();
      });
    } else {
      test.expect(1);
      test.ok(!err,"sync initialised OK");
      test.done();
    }
  });
};
