//by ALBERTO ARENA
//http://www.albertoarena.co.uk/a-simple-javascript-gettersetter/
 
define([], function () {
  return function(initValue, callback) {
    var val = initValue;
    if (typeof callback != 'function') {
      callback = null;
    }

    return function (v) {
      if (v !== undefined) {
        val = v;
        if (callback) {
          callback(v);
        }
      }
      return val;
    };
  };
});