//ARCGIS FRONT END CODE BEGINS --------------------------------------------------------------------------------------------------
/*
    Use this to first create a feature layer: Mapping APIs and Location Services: https://developers.arcgis.com/documentation/mapping-apis-and-services/data-hosting/tutorials/tools/define-a-new-feature-layer/
    Then use this to edit the feature layer and draw/modify a web map but only through GUI: ArcGIS Maps SDK for JS https://developers.arcgis.com/javascript/latest/tutorials/edit-feature-data/#prerequisites
    But then use this to add REST API usage so the code can do this without GUI: ArcGIS REST JS https://developers.arcgis.com/arcgis-rest-js/layers/edit-feature-data/ 
    NOTE: This is a light description on tooling: ArcGIS Maps SDK for JS https://developers.arcgis.com/javascript/latest/tooling-intro/
*/
require([
	"esri/config",
	"esri/WebMap",
	"esri/views/MapView",
	"esri/widgets/ScaleBar",
	"esri/widgets/Legend",

	"esri/layers/FeatureLayer",
	"esri/widgets/Editor",
], function (esriConfig, WebMap, MapView, ScaleBar, Legend, FeatureLayer, Editor) {
	// Feature Layer Things:
	// Reference a feature layer to edit
	const SOAR_GPS_DATA_FEATURE_LAYER = new FeatureLayer({
		url: "https://services1.arcgis.com/sQOueNHZBxRJjRN3/arcgis/rest/services/SOAR_GPS_Data_1/FeatureServer",
	});

	esriConfig.apiKey = "AAPK79842bdf715e40379281a8d783198227FFYy_xmKDj0ys_-3KxRmMiFhtsF2f8HH8TMIzVfoN26e_dAcRRgmYsnivKJMf20w";

	const webmap = new WebMap({
		portalItem: {
			id: "812c6c7bf59542d9a957220725962832",
		},
		layers: [SOAR_GPS_DATA_FEATURE_LAYER],
	});

	const view = new MapView({
		container: "viewDiv",
		map: webmap,
	});

	// Makes the map scale bar
	const scalebar = new ScaleBar({
		view: view,
	});
	view.ui.add(scalebar, "bottom-left");

	// Makes the Legend
	const legend = new Legend({
		view: view,
	});
	view.ui.add(legend, "top-right");

	// Editor Widget
	const editor = new Editor({
		view: view,
	});
	// Add widget to the view
	view.ui.add(editor, "top-right");
});

// ------------------------------------------------------------------------------------------------------------------------------
//FRONT END CODE BEGINS ---------------------------------------------------------------------------------------------------------

// Important Global Variables
let active_map = "";
let payload = false;

let varn_map = document.getElementById("varn_map");
let usf_map = document.getElementById("usf_map");
let palm_bay_map = document.getElementById("palm_bay_map");
let huntsville_map = document.getElementById("huntsville_map");

varn_map.addEventListener("click", () => {
	draw_varn();
	active_map = "Varn";
});
usf_map.addEventListener("click", () => {
	draw_usf();
	active_map = "USF";
});

// Sets Payload Global Variable
document.getElementById("rocket_identifier").addEventListener("click", () => {
	payload = false;
});
document.getElementById("payload_identifier").addEventListener("click", () => {
	payload = true;
});

//Code that plots values from a front-end input NMEA sentence
document.getElementById("dummy_plot").addEventListener("click", () => {
	let nmea = document.getElementById("live_nmea").value;
	console.log("hi");
	if (active_map == "USF") {
		plot_usf(nmea);
	}
	if (active_map == "Varn") {
		plot_varn(nmea);
	}
});

// Code that logs information passed to it to the Console on screen
let logging_object = document.getElementById("live_console");
let logger_reference_node = document.createTextNode("");
logging_object.appendChild(logger_reference_node);
function logger(information, error = false) {
	// Interestingly, the logger HTML element needed a CSS value of white-space: pre-line;
	// https://stackoverflow.com/questions/9980416/how-can-i-insert-new-line-carriage-returns-into-an-element-textcontent
	let new_text = document.createTextNode(information);

	let new_text_holder = document.createElement("div");
	new_text_holder.appendChild(new_text);
	new_text_holder.className = "live_console_messages_light";

	if (document.getElementById("theme_value").textContent == "Light") {
		new_text_holder.className = "live_console_messages_light";
	}

	if (document.getElementById("theme_value").textContent == "Dark") {
		new_text_holder.className = "live_console_messages_dark";
	}

	if (error == true) {
		if (document.getElementById("theme_value").textContent == "Light") {
			new_text_holder.classList.add("live_console_error_messages_light");
		}
		if (document.getElementById("theme_value").textContent == "Dark") {
			new_text_holder.classList.add("live_console_error_messages_dark");
		}
	}

	logging_object.insertBefore(new_text_holder, logger_reference_node);
	logger_reference_node = new_text_holder;
}

//WEBSOCKET CODE
var socket = io.connect("http://" + document.domain + ":" + location.port);

socket.on("gps_update", (thing) => {
	console.log(thing);
	console.log("GPS UPDATE", thing.section, thing.nmea);
	logger("GPS UPDATE", thing.section, thing.nmea);
	// console.log(data.nmea);
	// plot_varn(data.nmea);
	// plot_usf(data.nmea);

	if (active_map == "USF") {
		plot_usf(thing.section, thing.nmea);
	}
	if (active_map == "Varn") {
		plot_varn(thing.section, thing.nmea);
	}
});

socket.on("gps_message", function (data) {
	console.log(data);
	logger(data);
});

socket.on("log_lora", function (data) {
	console.log(data);
	logger(data);
});

//BACK END CODE BEGINS ----------------------------------------------------------------------------------------------------------
//Script to connect with the server
var server = "http://" + document.domain + ":" + location.port;

//Below script to handle GPS Start button press

// Checks if an entered COM port is valid or not
function isValidComPort(comPort) {
	var pattern = /^COM\d+$/;
	return pattern.test(comPort);
}

// Initiates connection to the back end
var generateBtn = document.getElementById("gps_start");
generateBtn.addEventListener("click", get_response);
function get_response() {
	console.log("Beginning GPS");
	let com_port = document.getElementById("com_port").value;
	if (isValidComPort(com_port) == false) {
		console.error(`Invalid COM Port: ${com_port}`);
		logger(`Invalid COM Port: ${com_port}`, true);
		document.getElementById("invalid_com_port_notif").style.display = "block";
		return;
	}
	if (isValidComPort(com_port) == true) {
		logger(`Valid COM Port: ${com_port}`);
		document.getElementById("invalid_com_port_notif").style.display = "none";
	}

	const fetch_promise = fetch(server + "/telemetry_start/" + com_port);
	fetch_promise
		.then((response) => {
			if (!response.ok) {
				throw new Error("HTTP error: ${response.status}");
			}
			return response.text();
		})
		.then((text) => {
			console.log(text);
			logger(text);
		});
}

document.getElementById("close_serial").addEventListener("click", () => {
	const fetch_promise = fetch(server + "/close_serial");
	fetch_promise
		.then((response) => {
			if (!response.ok) {
				throw new Error(`HTTP error: ${response.status}`);
			}
			return response.text();
		})
		.then((text) => {
			console.log(text);
			logger(text);
		});
});

function SendCommand(command, id = 0) {
	var full_command = `${command},${id}`;
	const fetch_promise = fetch(server + "/command/" + full_command);
	fetch_promise
		.then((response) => {
			if (!response.ok) {
				throw new Error(`HTTP error: ${response.status}`);
				logger(`HTTP error: ${response.status}`);
			}
			return response.text();
		})
		.then((text) => {
			console.log(text);
			logger(text);
		})
		.catch((err) => {
			logger(err, true);
			console.log(err);
		});
}
document.getElementById("live_console_send_msg_btn").addEventListener("click", () => {
	let message = document.getElementById("live_console_send_msg_field").value;
	let id = document.getElementById("live_console_send_msg_id").value;
	SendCommand(message, id);
});

//Sends a random message to the transmitter
var random_cmd = document.getElementById("random_cmd");
random_cmd.addEventListener("click", send_random);
function send_random() {
	const fetch_promise = fetch(server + "/random_cmd");
	fetch_promise
		.then((response) => {
			if (!response.ok) {
				throw new Error("HTTP error: ${response.status}");
			}
			return response.text();
		})
		.then((text) => {
			console.log(text);
		});
}

// ------------------------------------------------------------------------------------------------------------------------------
// ARCGIS REST CODE BEGINS-------------------------------------------------------------------------------------------------------
/* 
Use this to first create a feature layer: Mapping APIs and Location Services: https://developers.arcgis.com/documentation/mapping-apis-and-services/data-hosting/tutorials/tools/define-a-new-feature-layer/
Then use this to edit the feature layer and draw/modify a web map but only through GUI: ArcGIS Maps SDK for JS https://developers.arcgis.com/javascript/latest/tutorials/edit-feature-data/#prerequisites
But then use this to add REST API usage so the code can do this without GUI: ArcGIS REST JS https://developers.arcgis.com/arcgis-rest-js/layers/edit-feature-data/
NOTE: This is a light description on tooling: ArcGIS Maps SDK for JS https://developers.arcgis.com/javascript/latest/tooling-intro/ 
*/
console.log("IMPORTED");

//import {ApiKeyManager} from "/node_modules/@esri/arcgis-rest-request/dist/esm/index.js";
//
//console.log("IMPORTED");
//import {
//	addFeatures,
//	updateFeatures,
//	deleteFeatures,
//} from "/node_modules/@esri/arcgis-rest-feature-service/dist/esm/index.js";
//
//// import "/node_modules/@esri/arcgis-rest-request";
//// import "/node_modules/@esri/arcgis-rest-feature-service";
//
//console.log("IMPORTED");
//
//async function add_points(section, nmea_sentence) {
//	const apiKey = "AAPK79842bdf715e40379281a8d783198227FFYy_xmKDj0ys_-3KxRmMiFhtsF2f8HH8TMIzVfoN26e_dAcRRgmYsnivKJMf20w";
//
//	const authentication = ApiKeyManager.fromKey(apiKey);
//
//	const featureServiceLayerUrl =
//		"https://services1.arcgis.com/sQOueNHZBxRJjRN3/arcgis/rest/services/SOAR_GPS_Data_1/FeatureServer/0";
//	// Data cleansing
//	// Sample Strings:
//	// USF: <LORA>+RCV=7,31,GPS: A,2803.9643,N,08225.0154,W\r</LORA>
//	// Varn: (modified and updated) <LORA>+RCV=7,31,GPS: A,2805.5864,N,08210.5439,W\r</LORA>
//	// Old Sample: $GNRMC,025859.000,A,2805.5864,N,08210.5439,W,0.10,174.77,181123,,,A*68
//
//	//Grab the important information from the NMEA sentence
//	/*WARNING--------------------------------------------------
//        WHEN USING THE INPUT FIELD TO ENTER IN SAMPLE NMEA SENTENCE FORMATS
//        MAKE SURE TO INCLUDE THE SPACE BEFORE THE SAMPLE
//        So for example, if you had the following sentence that I will surround in quotes
//        "A,2803.9643,N,08225.0154,W\r"
//        Then when entering it into the field, you MUST enter it like this:
//        " A,2803.9643,N,08225.0154,W\r"
//        This will have a space at the front of whatever you enter
//        Another example, let us say that you wished to enter the word "Hi"
//        then you would enter " Hi"
//    */
//	// A,2805.5864,N,08210.5439,W\r
//	// let payload = section;
//	// let nmea_sentence = " A,2805.5864,N,08210.5439,W\r";
//	let status = nmea_sentence.slice(1, 2);
//
//	if (status == "V") {
//		console.error("Invalid GPS String: " + status);
//		logger("Invalid GPS String: " + status, true);
//		return;
//	}
//	if (status == "A") {
//		console.log("Valid GPS String: " + status);
//		logger("Valid GPS String: " + status);
//	}
//
//	let minuteslat = nmea_sentence.slice(5, 12);
//	let minuteslong = nmea_sentence.slice(18, 25);
//	let total_lat = nmea_sentence.slice(3, 14);
//	let total_long = nmea_sentence.slice(15, 27);
//
//	// <LORA>+RCV=7,31,GPS: A,2803.9643,N,08225.0154,W\r</LORA>
//	/*
//	Here is a sample NMEA sentence from Varn Ranch that can be used to test functioning
//    $GNRMC,025859.000,A,2805.5864,N,08210.5439,W,0.10,174.77,181123,,,A*68
//    NOTE: The GPS gives data in the following format:
//    Latitude: DDMM.MMMM (D representing Degrees, and M for minutes)
//    Longitude: DDDMM.MMMM (D representing Degrees, and M for minutes)
//    Understand that each Degree has 60 Minutes, and each Minute has 60 Seconds
//	*/
//
//	// Prepare the coordinates for display in the browser page in emergency scenarios ----------
//	//The optimal format of output would be in DD°MM'SS'
//	let display_degrees_lat = total_lat.slice(0, 2);
//	let display_minutes_lat = total_lat.slice(2, 4);
//	let old_display_seconds_lat = total_lat.slice(4, 9);
//	old_display_seconds_lat = old_display_seconds_lat * 60;
//	let display_seconds_lat = `${old_display_seconds_lat}`;
//	display_seconds_lat = display_seconds_lat.slice(0, 6);
//	Number.parseFloat(display_seconds_lat);
//	let display_direction_lat = total_lat.slice(10, 11);
//
//	let display_degrees_long = total_long.slice(0, 3);
//	let display_minutes_long = total_long.slice(3, 5);
//	let old_display_seconds_long = total_long.slice(5, 10);
//	old_display_seconds_long = old_display_seconds_long * 60;
//	let display_seconds_long = `${old_display_seconds_long}`;
//	display_seconds_long = display_seconds_long.slice(0, 6);
//	Number.parseFloat(display_seconds_long);
//	let display_direction_long = total_long.slice(11, 12);
//
//	let display_dms_lat = `${display_degrees_lat}°${display_minutes_lat}'${display_seconds_lat}"${display_direction_lat}`;
//	let display_dms_long = `${display_degrees_long}°${display_minutes_long}'${display_seconds_long}"${display_direction_long}`;
//	console.log(`DMS\nLatitude: ${display_dms_lat}\nLongitude: ${display_dms_long}`);
//	logger(`DMS\nLatitude: ${display_dms_lat}\nLongitude: ${display_dms_long}`);
//	// --------------------------------------------------------------------------------------------
//
//	//Display on the side display on the browser: -------------------------------------------------
//	let side_display_lat = undefined;
//	let side_display_long = undefined;
//	if (section == false) {
//		// The GPS Statement is for the rocket
//		side_display_lat = document.getElementById("side_display_lat_rocket");
//		side_display_long = document.getElementById("side_display_long_rocket");
//	} else {
//		if (section == true) {
//			// The GPS Statement is for the payload
//			side_display_lat = document.getElementById("side_display_lat_payload");
//			side_display_long = document.getElementById("side_display_long_payload");
//		}
//	}
//	let side_lat_info = document.createTextNode(display_dms_lat);
//	side_display_lat.textContent = "";
//	side_display_lat.textContent = "Latitude: ";
//	side_display_lat.appendChild(side_lat_info);
//
//	let side_long_info = document.createTextNode(display_dms_long);
//	side_display_long.textContent = "";
//	side_display_long.textContent = "Longitude: ";
//	side_display_long.appendChild(side_long_info);
//	// --------------------------------------------------------------------------------------------
//
//	// Take DMS to just Degrees, no minutes or seconds --------------------------------------------
//	let new_lat_mins = parseFloat(display_minutes_lat) + parseFloat(display_seconds_lat) / 60;
//	let just_degrees_lat = parseFloat(display_degrees_lat) + parseFloat(new_lat_mins) / 60;
//
//	let new_long_mins = parseFloat(display_minutes_long) + parseFloat(display_seconds_long) / 60;
//	let just_degrees_long = parseFloat(display_degrees_long) + parseFloat(new_long_mins) / 60;
//
//	if (display_direction_lat == "S") {
//		just_degrees_lat = parseFloat(just_degrees_lat) * -1;
//	}
//	if (display_direction_long == "W") {
//		just_degrees_long = parseFloat(just_degrees_long) * -1;
//	}
//	console.log(`Degrees Only\nLatitude: ${just_degrees_lat}\nLongitude: ${just_degrees_long}`);
//	logger(`Degrees Only\nLatitude: ${just_degrees_lat}\nLongitude: ${just_degrees_long}`);
//	// --------------------------------------------------------------------------------------------
//
//	// Plot with ArcGIS REST API ------------------------------------------------------------------
//	// Produce a unique ID and name and ID the section of the rocket
//	let part = undefined;
//	if (section == false) {
//		part = "Rocket";
//	} else {
//		part = "Payload";
//	}
//
//	let date = new Date().toJSON();
//
//	// ArcGIS REST API Work
//	const pointsToAdd = {
//		attributes: {
//			id: 1.2,
//			name: date,
//			rckt_section: part,
//		},
//		geometry: {
//			// x: -82.17573167,
//			// y: 28.09310667,
//			x: just_degrees_long,
//			y: just_degrees_lat,
//			spatialReference: {
//				wkid: 4326,
//			},
//		},
//		style: {},
//	};
//	let add_response = await addFeatures({
//		url: featureServiceLayerUrl,
//		features: [pointsToAdd],
//		authentication,
//	})
//		// .then(async (response) => {
//		// 	if (!response.addResults[0].success) {
//		// 		// stop early if adding a new feature was not successful
//		// 		return;
//		// 	}
//
//		// 	const featureToUpdate = {
//		// 		attributes: {
//		// 			objectId: response.addResults[0].objectId,
//		// 			id: 2,
//		// 			name: "Updated Data Point 2",
//		// 		},
//		// 	};
//
//		// 	const updateResponse = await updateFeatures({
//		// 		url: featureServiceLayerUrl,
//		// 		features: [featureToUpdate],
//		// 		authentication,
//		// 	});
//		// 	console.log(updateResponse);
//		// })
//
//		.then((response) => {
//			console.log(response);
//			logger(response);
//		});
//	// .then(async (response) => {
//	// 	console.log(response);
//
//	// 	if (!response.addResults[0].success) {
//	// 		// stop early if adding a new feature was not successful
//	// 		return;
//	// 	}
//
//	// 	// perform an update to feature attributes using the objectId of the newly added feature
//	// 	const featureToUpdate = {
//	// 		attributes: {
//	// 			objectId: response.addResults[0].objectId,
//	// 			id: 2,
//	// 			name: "Updated Data Point 2",
//	// 		},
//	// 	};
//
//	console.log(`The function: ${add_response}`);
//}
//
//// DEVELOPER TESTING CODE -----------------------------------------------------
//// Section variable is false means the string is for the rocket
//// Section variable is true means the string is for the payload
//// Sets Payload Global Variable
//
//// This code is for pressing buttons and plotting fake gps sentences to test functionality on gps_data.html
//
//// Arbitrarily lets you designate a fake sentence as either for payload or the rocket
//let rocket = false;
//let rocket_identifier = document.getElementById("rocket_identifier");
//if (rocket_identifier != null) {
//	rocket_identifier.addEventListener("click", () => {
//		rocket = true;
//	});
//}
//let payload_identifier = document.getElementById("payload_identifier");
//if (payload_identifier != null) {
//	payload_identifier.addEventListener("click", () => {
//		rocket = false;
//	});
//}
//
//// Plots fake nmea sentence
//let dummy_plot = document.getElementById("dummy_plot");
//if (dummy_plot != null) {
//	dummy_plot.addEventListener("click", () => {
//		let nmea = document.getElementById("live_nmea").value;
//		add_points(rocket, nmea);
//	});
//}
//// END DEVELOPER TESTING CODE -------------------------------------------------
//
//// GPS DATA Socket.io Listener --------------------------------------
//// This will take data from the event generated by parser.py
//// and feed it to the ARCGIS Rest functions below
//socket.on("gps_update", (thing) => {
//	console.log(
//		`GPS UPDATE:\nThis is from the Socket.io event listener\nRocket Section: ${thing.section}\nFalse means Rocket, True means Payload\nOutput Received: ${thing.nmea_sentence}`,
//	);
//	logger(
//		`GPS UPDATE:\nThis is from the Socket.io event listener\nRocket Section: ${thing.section}\nFalse means Rocket, True means Payload\nOutput Received: ${thing.nmea_sentence}`,
//	);
//	add_points(thing.section, thing.nmea_sentence);
//});
//
//// --------------------------------------------------------------------------------------------------------------------
//// Code that can be used with ArcGIS REST API to update and modify data values should the need arise
//// const featureToAdd = {
//// 	attributes: {
//// 		id: 2,
//// 		name: "Data Point 2",
//// 	},
//// 	geometry: {
//// 		x: -118.807,
//// 		y: 34.002,
//// 		spatialReference: {
//// 			wkid: 4326,
//// 		},
//// 	},
//// };
//
//// begin by adding a new feature to the feature service layer,
//// then update its attributes,
//// and finally delete it from the layer
//
//// begin by adding a new feature to the feature service layer,
//// then update its attributes,
//// and finally delete it from the layer
//
//// addFeatures({
//// 	url: featureServiceLayerUrl,
//// 	features: [featureToAdd],
//// 	authentication,
//// }).then(async (response) => {
//// 	console.log(response);
//
//// 	if (!response.addResults[0].success) {
//// 		// stop early if adding a new feature was not successful
//// 		return;
//// 	}
//
//// 	// perform an update to feature attributes using the objectId of the newly added feature
//// 	const featureToUpdate = {
//// 		attributes: {
//// 			objectId: response.addResults[0].objectId,
//// 			id: 2,
//// 			name: "Updated Data Point 2",
//// 		},
//// 	};
//
//// 	const updateResponse = await updateFeatures({
//// 		url: featureServiceLayerUrl,
//// 		features: [featureToUpdate],
//// 		authentication,
//// 	});
//
//// 	console.log(updateResponse);
//
//// 	if (!updateResponse.updateResults[0].success) {
//// 		// stop early if updating the feature was not successful
//// 		return;
//// 	}
//
//// 	// delete the feature using the objectId of the updated feature
//// 	// NOTE: it is the same objectId provided in the response of `addFeatures`
//// 	const featureToDelete = [updateResponse.updateResults[0].objectId];
//
//// 	const deleteResponse = await deleteFeatures({
//// 		url: featureServiceLayerUrl,
//// 		objectIds: [featureToDelete],
//// 		authentication,
//// 	});
//
//// 	console.log(deleteResponse);
//// });
//
///*
//// Declare the chart dimensions and margins.
//const width = 640;
//const height = 400;
//const marginTop = 20;
//const marginRight = 20;
//const marginBottom = 30;
//const marginLeft = 40;
//
//// Declare the x (horizontal position) scale.
//const x = d3
//	.scaleUtc()
//	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
//	.range([marginLeft, width - marginRight]);
//
//// Declare the y (vertical position) scale.
//const y = d3
//	.scaleLinear()
//	.domain([0, 100])
//	.range([height - marginBottom, marginTop]);
//
//// Create the SVG container.
//const svg = d3.create("svg").attr("width", width).attr("height", height);
//
//// Add the x-axis.
//svg
//	.append("g")
//	.attr("transform", `translate(0,${height - marginBottom})`)
//	.call(d3.axisBottom(x));
//
//// Add the y-axis.
//svg.append("g").attr("transform", `translate(${marginLeft},0)`).call(d3.axisLeft(y));
//
//// Append the SVG element.
//let holder = document.createElement("div");
//holder.append(svg.node());
//return holder;
//}
//*/
//