////////////////////////////////////////////////////////////////////////////////////////////////////
// Timeline Pins
//

// pins
var curDate = new Date();

// "Exercised" pin
var exercisedPIN = {
  "id": String.prototype.concat("7-min-workout-exercised", Math.random().toString(36).substring(10)),
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
  "time": curDate.toISOString(),
  "layout": {
    "type": "genericPin",
    "title": "Exercise!",
    "subtitle": "Remember to do your 7 Min Workout.",
    "tinyIcon": "system://images/SCHEDULED_EVENT",
    "body": "C'mon! What are you waiting for? It's only 7 min 50 sec long :)"
  }
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Timeline Library
//

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

// send a series of pins to the timeline
function timelineUpdatePins() {
    // get time to remind
    var date = curDate;
    date.setHours(localStorage['reminder_time']);
    // check if enabled
    if (localStorage['reminder_enabled'] != true) {
        date.setDate(-7);
        return;
    }
    // update pins
    for (var ii = 0; ii < 7; ii++) {
        // insert pin
        reminderPIN.time = date;
        insertUserPin(reminderPIN, function (responseText) {
            console.log('Pin Sent Result: ' + responseText);
        });
        // step date to tomorrow
        date.setDate(1);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Configuration Page
//

// Show the configuration page when the "gear" icon is clicked
Pebble.addEventListener('showConfiguration', function() {
    var url = 'https://yclepticstudios.github.io/pebble-7-min/config/index.html';
    console.log('Showing configuration page: ' + url);

    Pebble.openURL(url);
});

// Return data to C application when configuration page is closed
Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration page returned: ' + JSON.stringify(configData));

    // save to persistent storage
    localStorage['reminder_enabled'] = configData['reminder_enabled'];
    localStorage['reminder_time'] = configData['reminder_time'];
});


////////////////////////////////////////////////////////////////////////////////////////////////////
// App Message
//

// message received
Pebble.addEventListener('appmessage', function(e) {
  // check for key
  if (e.payload.hasOwnProperty('KEY_EXERSIZED')){
    // check that it is valid to send pins i.e. its SDK 3.0 or greater
    if (typeof Pebble.getTimelineToken == 'function') {
      // insert pin
      insertUserPin(exercisedPIN, function (responseText) {
        console.log('Pin Sent Result: ' + responseText);
      });
    }
  }
});

// loaded and ready
Pebble.addEventListener('ready', function(e) {
  console.log("JS ready!");
});