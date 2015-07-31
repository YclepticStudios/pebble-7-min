// ********** Timeline ********** //
// pins
var curDate = new Date();
// "Exersized" pin
var exersizedPIN = {
  "id": String.prototype.concat("7-min-workout-exersized", Math.random().toString(36).substring(10)),
  "time": curDate.toISOString(),
  "layout": {
    "type": "genericPin",
    "title": "Workout Done!",
    "subtitle": "You did your 7 Min Workout at this time.",
    "tinyIcon": "system://images/REACHED_FITNESS_GOAL",
    "body": "Nice job staying healthy! Keep up the great work ;)"
  }
};
// "Reminder" pin
var reminderPIN = {
  "id": "7-min-workout-reminder",
  "time": "curDate.toISOString()",
  "layout": {
    "type": "genericPin",
    "title": "Exersize!",
    "subtitle": "Remember to do your 7 Min Workout.",
    "tinyIcon": "system://images/SCHEDULED_EVENT",
    "body": "C'mon! What are you waiting for? It's only 7 min 50 sec long :)"
  }
};

// ***** Timeline Lib ***** //
// The Timeline public URL root
var API_URL_ROOT = 'https://timeline-api.getpebble.com/';
/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, callback) {
    // User or shared?
    var url = API_URL_ROOT + 'v1/user/pins/' + pin.id;

    // Create XHR
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        console.log('timeline: response received: ' + this.responseText);
        callback(this.responseText);
    };
    xhr.open(type, url);

    // Get token
    Pebble.getTimelineToken(function (token) {
        // Add headers
        xhr.setRequestHeader('Content-Type', 'application/json');
        xhr.setRequestHeader('X-User-Token', '' + token);

        // Send
        xhr.send(JSON.stringify(pin));
        console.log('timeline: request sent.');
    }, function (error) { console.log('timeline: error getting timeline token: ' + error); });
}
/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
    timelineRequest(pin, 'PUT', callback);
}
/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function deleteUserPin(pin, callback) {
    timelineRequest(pin, 'DELETE', callback);
}



// ********** AppMessage ********** //
// send message to phone
function send_to_phone(){
  // create dictionary
  var dict = {
    'KEY_EXERSIZED':0
  };

  // send to pebble
  Pebble.sendAppMessage(dict,
    function(e) {
      console.log('Send successful.');
    },
    function(e) {
      console.log('Send failed!');
    }
  );
}

// message received
Pebble.addEventListener('appmessage', function(e) {
  // check for key
  if (e.payload.hasOwnProperty('KEY_EXERSIZED')){
    // check that it is valid to send pins i.e. its SDK 3.0 or greater
    if (typeof Pebble.getTimelineToken == 'function') {
      // insert pin
      insertUserPin(exersizedPIN, function (responseText) {
        console.log('Pin Sent Result: ' + responseText);
      });
    }
  }
});

// loaded and ready
Pebble.addEventListener('ready', function(e) {
  console.log("JS ready!");
});