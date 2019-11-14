var mConfig = {};
var mConfigURL = "http://www.mirz.com/91dubv3/index.10.html";
//var mConfigURL = "http://localhost:8080/index.10.html";

Pebble.addEventListener("ready", function(e) {
  loadLocalData();
  returnConfigToPebble();
});

Pebble.addEventListener('appmessage', function(e) {
    //console.log('Received message: ' + JSON.stringify(e.payload));
});

Pebble.addEventListener("showConfiguration", function(e) {
	var url = mConfigURL;
	if(Pebble.getActiveWatchInfo) {
		url += '?platform=' + Pebble.getActiveWatchInfo().platform;
	}
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
    if (e.response) {
      var config = JSON.parse(decodeURIComponent(e.response));
      saveLocalData(config);
      returnConfigToPebble();
    }
});

function loadLocalData() {
	try {
		mConfig = JSON.parse(localStorage.getItem("config"));
	}
	catch(err) {
		mConfig = {};
	}
}

function saveLocalData(config) {
	localStorage.setItem("config", JSON.stringify(config));
  loadLocalData();
}

function returnConfigToPebble() {
	if(mConfig === null) {
		mConfig = {};
	}
  //console.log("Configuration window returned: " + JSON.stringify(mConfig));
	Pebble.sendAppMessage(mConfig);
}
