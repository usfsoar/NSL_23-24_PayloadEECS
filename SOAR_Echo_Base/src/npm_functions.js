import * as d3 from "d3";
// import * as hi from "control_panel.html";
// export {d3_draw_chart};

import * as io from "/static/socket.io.js";

// import * as featureServiceTools from "@esri/arcgis-rest-feature-service";

//WEBSOCKET CODE
var socket = io.connect("http://" + document.domain + ":" + location.port);

socket.on("alti_update", function (data) {
	console.log("ALTIMETER UPDATE", data);
	logger("ALTIMETER UPDATE", data);
});

socket.on("gps_message", function (data) {
	console.log(data);
	logger(data);
});
/*
function d3_draw_altitude_chart_rocket() {
	let hi = false;
	const altitudeData = [
		{time: 0, altitude: 0},
		{time: 5, altitude: 100},
		{time: 10, altitude: 200},
		{time: 15, altitude: 150},
		{time: 20, altitude: 300},
		{time: 30, altitude: 700},
		// Add more data points as needed
	];
	// import * as d3 from "https://cdn.jsdelivr.net/npm/d3@7/+esm";
	// Sample altitude data for demonstration

	// Set up chart dimensions
	const width = 600;
	const height = 400;
	const margin = {top: 20, right: 20, bottom: 30, left: 50};

	// Create SVG container
	let svg = d3
		.select("body")
		.append("svg")
		.attr("width", width + margin.left + margin.right)
		.attr("height", height + margin.top + margin.bottom)
		.append("g")
		.attr("transform", `translate(${margin.left},${margin.top})`);

	// Define scales
	let xScale = d3.scaleLinear().domain([0, 90]).range([0, width]);
	let yScale = d3
		.scaleLinear()
		.domain([0, d3.max(altitudeData, (d) => d.altitude)])
		.range([height, 0]);

	// Define line function
	let line = d3
		.line()
		.x((d) => xScale(d.time))
		.y((d) => yScale(d.altitude));

	// Add line to the chart
	svg
		.append("path")
		.datum(altitudeData)
		.attr("fill", "none")
		.attr("stroke", "steelblue")
		.attr("stroke-width", 2)
		.attr("d", line);

	// Add x-axis
	svg.append("g").attr("transform", `translate(0,${height})`).attr("class", "x-axis").call(d3.axisBottom(xScale));

	// Add y-axis
	svg.append("g").attr("class", "y-axis").call(d3.axisLeft(yScale));

	// Add tooltips
	let tooltip = svg.append("g").attr("class", "tooltip").style("display", "none");

	tooltip
		.append("rect")
		.attr("width", 60)
		.attr("height", 20)
		.attr("fill", "white")
		.style("stroke", "steelblue")
		.style("stroke-width", 2);

	let tooltipText = tooltip
		.append("text")
		.attr("x", 30)
		.attr("y", 10)
		.attr("text-anchor", "middle")
		.attr("dy", "0.35em")
		.style("fill", "steelblue")
		.style("font-size", "12px");

	// Add event listeners for tooltips
	svg
		.selectAll("circle")
		.data(altitudeData)
		.enter()
		.append("circle")
		.attr("cx", (d) => xScale(d.time))
		.attr("cy", (d) => yScale(d.altitude))
		.attr("r", 5)
		.style("fill", "steelblue")
		.on("mouseover", function (event, d) {
			tooltip.style("display", null);
			tooltip.attr("transform", `translate(${xScale(d.time)},${yScale(d.altitude)})`);
			tooltipText.text(d.altitude);
		})
		.on("mouseout", function () {
			tooltip.style("display", "none");
		});

	// document.getElementById("hi").addEventListener("click", () => {
	if ((hi = true)) {
		altitudeData.push({time: 35, altitude: 900});
		console.log(altitudeData);
		// Update scales with the new data range
		// xScale.domain([0, d3.max(altitudeData, d => d.time)]);
		yScale.domain([0, d3.max(altitudeData, (d) => d.altitude)]);

		// Select the existing line and update the data
		svg
			.select("path")
			.datum(altitudeData)
			.transition()
			.duration(250) // Set a transition duration for a smooth update
			.attr("d", line);

		// // Update the x-axis
		// svg.select('.x-axis')
		// .transition()
		// .duration(250)
		// .call(d3.axisBottom(xScale));

		// Update the y-axis
		svg.select(".y-axis").transition().duration(250).call(d3.axisLeft(yScale));

		// Select existing circles and bind new data
		const circles = svg.selectAll("circle").data(altitudeData);

		// Update existing circles
		circles
			.transition()
			.duration(250)
			.attr("cx", (d) => xScale(d.time))
			.attr("cy", (d) => yScale(d.altitude));

		// Add new circles
		circles
			.enter()
			.append("circle")
			.attr("cx", (d) => xScale(d.time))
			.attr("cy", (d) => yScale(d.altitude))
			.attr("r", 5)
			.style("fill", "steelblue")
			.on("mouseover", function (event, d) {
				tooltip.style("display", null);
				tooltip.attr("transform", `translate(${xScale(d.time)},${yScale(d.altitude)})`);
				tooltipText.text(d.altitude);
			})
			.on("mouseout", function () {
				tooltip.style("display", "none");
			});

		// Remove any circles not present in the new data
		circles.exit().remove();
	}
	// Append the SVG element.
	let holder = document.createElement("div");
	holder.append(svg.node());
	return holder;
}*/

function d3_draw_velocity_chart_rocket() {
	// Declare the chart dimensions and margins.
	const width = 640;
	const height = 400;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	const x = d3
		.scaleUtc()
		.domain([new Date("2023-01-01"), new Date("2024-01-01")])
		.range([marginLeft, width - marginRight]);

	// Declare the y (vertical position) scale.
	const y = d3
		.scaleLinear()
		.domain([0, 100])
		.range([height - marginBottom, marginTop]);

	// Create the SVG container.
	const svg = d3.create("svg").attr("width", width).attr("height", height);

	// Add the x-axis.
	svg
		.append("g")
		.attr("transform", `translate(0,${height - marginBottom})`)
		.call(d3.axisBottom(x));

	// Add the y-axis.
	svg.append("g").attr("transform", `translate(${marginLeft},0)`).call(d3.axisLeft(y));

	// Append the SVG element.
	let holder = document.createElement("div");
	holder.append(svg.node());
	return holder;
}

function d3_draw_acceleration_chart_rocket() {
	// Declare the chart dimensions and margins.
	const width = 640;
	const height = 400;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	const x = d3
		.scaleUtc()
		.domain([new Date("2023-01-01"), new Date("2024-01-01")])
		.range([marginLeft, width - marginRight]);

	// Declare the y (vertical position) scale.
	const y = d3
		.scaleLinear()
		.domain([0, 100])
		.range([height - marginBottom, marginTop]);

	// Create the SVG container.
	const svg = d3.create("svg").attr("width", width).attr("height", height);

	// Add the x-axis.
	svg
		.append("g")
		.attr("transform", `translate(0,${height - marginBottom})`)
		.call(d3.axisBottom(x));

	// Add the y-axis.
	svg.append("g").attr("transform", `translate(${marginLeft},0)`).call(d3.axisLeft(y));

	// Append the SVG element.
	let holder = document.createElement("div");
	holder.append(svg.node());
	return holder;
}

function d3_draw_altitude_chart_payload() {
	// Declare the chart dimensions and margins.
	const width = 640;
	const height = 400;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	const x = d3
		.scaleUtc()
		.domain([new Date("2023-01-01"), new Date("2024-01-01")])
		.range([marginLeft, width - marginRight]);

	// Declare the y (vertical position) scale.
	const y = d3
		.scaleLinear()
		.domain([0, 100])
		.range([height - marginBottom, marginTop]);

	// Create the SVG container.
	const svg = d3.create("svg").attr("width", width).attr("height", height);

	// Add the x-axis.
	svg
		.append("g")
		.attr("transform", `translate(0,${height - marginBottom})`)
		.call(d3.axisBottom(x));

	// Add the y-axis.
	svg.append("g").attr("transform", `translate(${marginLeft},0)`).call(d3.axisLeft(y));

	// Append the SVG element.
	let holder = document.createElement("div");
	holder.append(svg.node());
	return holder;
}

function d3_draw_velocity_chart_payload() {
	// Declare the chart dimensions and margins.
	const width = 640;
	const height = 400;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	const x = d3
		.scaleUtc()
		.domain([new Date("2023-01-01"), new Date("2024-01-01")])
		.range([marginLeft, width - marginRight]);

	// Declare the y (vertical position) scale.
	const y = d3
		.scaleLinear()
		.domain([0, 100])
		.range([height - marginBottom, marginTop]);

	// Create the SVG container.
	const svg = d3.create("svg").attr("width", width).attr("height", height);

	// Add the x-axis.
	svg
		.append("g")
		.attr("transform", `translate(0,${height - marginBottom})`)
		.call(d3.axisBottom(x));

	// Add the y-axis.
	svg.append("g").attr("transform", `translate(${marginLeft},0)`).call(d3.axisLeft(y));

	// Append the SVG element.
	let holder = document.createElement("div");
	holder.append(svg.node());
	return holder;
}

function d3_draw_acceleration_chart_payload() {
	// Declare the chart dimensions and margins.
	const width = 640;
	const height = 400;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	const x = d3
		.scaleUtc()
		.domain([new Date("2023-01-01"), new Date("2024-01-01")])
		.range([marginLeft, width - marginRight]);

	// Declare the y (vertical position) scale.
	const y = d3
		.scaleLinear()
		.domain([0, 100])
		.range([height - marginBottom, marginTop]);

	// Create the SVG container.
	const svg = d3.create("svg").attr("width", width).attr("height", height);

	// Add the x-axis.
	svg
		.append("g")
		.attr("transform", `translate(0,${height - marginBottom})`)
		.call(d3.axisBottom(x));

	// Add the y-axis.
	svg.append("g").attr("transform", `translate(${marginLeft},0)`).call(d3.axisLeft(y));

	// Append the SVG element.
	let holder = document.createElement("div");
	holder.append(svg.node());
	return holder;
}

// GPS DATA CODE-------------------------------------------------------------------------------------------------------
// Logger Function --------------------------------------------------
// Copied exactly as it is from gps_data.html
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
	new_text_holder.className = "live_console_messages";
	if (error == true) {
		new_text_holder.classList.add("live_console_error_messages");
	}

	logging_object.insertBefore(new_text_holder, logger_reference_node);
	logger_reference_node = new_text_holder;
}

// ARCGIS REST ------------------------------------------------------
// Use this to first create a feature layer: Mapping APIs and Location Services: https://developers.arcgis.com/documentation/mapping-apis-and-services/data-hosting/tutorials/tools/define-a-new-feature-layer/
// Then use this to edit the feature layer and draw/modify a web map but only through GUI: ArcGIS Maps SDK for JS https://developers.arcgis.com/javascript/latest/tutorials/edit-feature-data/#prerequisites
// But then use this to add REST API usage so the code can do this without GUI: ArcGIS REST JS https://developers.arcgis.com/arcgis-rest-js/layers/edit-feature-data/
// NOTE: This is a light description on tooling: ArcGIS Maps SDK for JS https://developers.arcgis.com/javascript/latest/tooling-intro/

import {ApiKeyManager} from "@esri/arcgis-rest-request";
import {addFeatures, updateFeatures, deleteFeatures} from "@esri/arcgis-rest-feature-service";

const apiKey = "AAPK79842bdf715e40379281a8d783198227FFYy_xmKDj0ys_-3KxRmMiFhtsF2f8HH8TMIzVfoN26e_dAcRRgmYsnivKJMf20w";

const authentication = ApiKeyManager.fromKey(apiKey);

const featureServiceLayerUrl =
	"https://services1.arcgis.com/sQOueNHZBxRJjRN3/arcgis/rest/services/SOAR_GPS_Data_1/FeatureServer/0";

async function add_points(rocket, nmea_sentence) {
	// Data cleansing
	// let payload = section;
	// let nmea_sentence = " A,2805.5864,N,08210.5439,W\r";
	let status = nmea_sentence.slice(1, 2);

	if (status == "V") {
		console.error("Invalid GPS String: " + status);
		logger("Invalid GPS String: " + status, true);
		return;
	}
	if (status == "A") {
		console.log("Valid GPS String: " + status);
		logger("Valid GPS String: " + status);
	}

	let minuteslat = nmea_sentence.slice(5, 12);
	let minuteslong = nmea_sentence.slice(18, 25);
	let total_lat = nmea_sentence.slice(3, 14);
	let total_long = nmea_sentence.slice(15, 27);

	// <LORA>+RCV=7,31,GPS: A,2803.9643,N,08225.0154,W\r</LORA>
	// This code transforms the GPS coordinates into a home-made pixel measurements to draw
	// from a predetermined reference point so it can be drawn on a map
	// let real_lat = (6076.12 * (minuteslat - 3.9643)) / (300 / 41);
	// let real_long = (6076.12 * Math.cos(real_lat) * (minuteslong - 25.0154)) / (300 / 41);
	// console.log(`Pixels from Origin:\nLat: ${real_lat}\nLong: ${real_long}`);
	// logger(`Pixels from Origin:\nLat: ${real_lat}\nLong: ${real_long}`);

	// Optimize the total_lat and total_long variables for display in emergency scenarios
	//The optimal format of output would be in DD°MM'SS'
	let display_degrees_lat = total_lat.slice(0, 2);
	let display_minutes_lat = total_lat.slice(2, 4);
	let old_display_seconds_lat = total_lat.slice(4, 9);
	old_display_seconds_lat = old_display_seconds_lat * 60;
	let display_seconds_lat = `${old_display_seconds_lat}`;
	display_seconds_lat = display_seconds_lat.slice(0, 6);
	Number.parseFloat(display_seconds_lat);
	let display_direction_lat = total_lat.slice(10, 11);

	let display_degrees_long = total_long.slice(0, 3);
	let display_minutes_long = total_long.slice(3, 5);
	let old_display_seconds_long = total_long.slice(5, 10);
	old_display_seconds_long = old_display_seconds_long * 60;
	let display_seconds_long = `${old_display_seconds_long}`;
	display_seconds_long = display_seconds_long.slice(0, 6);
	Number.parseFloat(display_seconds_long);
	let display_direction_long = total_long.slice(11, 12);

	let display_dms_lat = `${display_degrees_lat}°${display_minutes_lat}'${display_seconds_lat}"${display_direction_lat}`;
	let display_dms_long = `${display_degrees_long}°${display_minutes_long}'${display_seconds_long}"${display_direction_long}`;
	console.log(`DMS\nLatitude: ${display_dms_lat}\nLongitude: ${display_dms_long}`);
	logger(`DMS\nLatitude: ${display_dms_lat}\nLongitude: ${display_dms_long}`);

	//Display on the coordinate the received information:
	let side_display_lat = undefined;
	let side_display_long = undefined;
	if (rocket == false) {
		// The GPS Statement is for the payload
		side_display_lat = document.getElementById("side_display_lat_payload");
		side_display_long = document.getElementById("side_display_long_payload");
	} else {
		if (rocket == true) {
			// The GPS Statement is for the rocket
			side_display_lat = document.getElementById("side_display_lat_rocket");
			side_display_long = document.getElementById("side_display_long_rocket");
		}
	}
	let side_lat_info = document.createTextNode(display_dms_lat);
	side_display_lat.textContent = "";
	side_display_lat.textContent = "Latitude: ";
	side_display_lat.appendChild(side_lat_info);

	let side_long_info = document.createTextNode(display_dms_long);
	side_display_long.textContent = "";
	side_display_long.textContent = "Longitude: ";
	side_display_long.appendChild(side_long_info);

	//NOTE: Need to add a way to place points where I dictate

	// Take DMS to just Degrees
	let new_lat_mins = parseFloat(display_minutes_lat) + parseFloat(display_seconds_lat) / 60;
	let just_degrees_lat = parseFloat(display_degrees_lat) + parseFloat(new_lat_mins) / 60;

	let new_long_mins = parseFloat(display_minutes_long) + parseFloat(display_seconds_long) / 60;
	let just_degrees_long = parseFloat(display_degrees_long) + parseFloat(new_long_mins) / 60;

	if (display_direction_lat == "S") {
		just_degrees_lat = parseFloat(just_degrees_lat) * -1;
	}
	if (display_direction_long == "W") {
		just_degrees_long = parseFloat(just_degrees_long) * -1;
	}
	console.log(`Degrees Only\nLatitude: ${just_degrees_lat}\nLongitude: ${just_degrees_long}`);
	logger(`Degrees Only\nLatitude: ${just_degrees_lat}\nLongitude: ${just_degrees_long}`);

	// Produce a unique ID and name and ID the section of the rocket
	let part = undefined;
	if (rocket == false) {
		part = "Payload";
	} else {
		part = "Rocket";
	}

	let date = new Date().toJSON();

	// ArcGIS REST API Work
	const pointsToAdd = {
		attributes: {
			id: 1.2,
			name: date,
			rckt_section: part,
		},
		geometry: {
			// x: -82.17573167,
			// y: 28.09310667,
			x: just_degrees_long,
			y: just_degrees_lat,
			spatialReference: {
				wkid: 4326,
			},
		},
	};
	let add_response = await addFeatures({
		url: featureServiceLayerUrl,
		features: [pointsToAdd],
		authentication,
	}).then((response) => {
		console.log(response);
		logger(response);
	});

	console.log(`The function: ${add_response}`);
}

// Sets Payload Global Variable
// Rocket variable is true means the string is for the rocket
// Rocket variable is false means the string is for the payload
let rocket = false;
let rocket_identifier = document.getElementById("rocket_identifier");
if (rocket_identifier != null) {
	rocket_identifier.addEventListener("click", () => {
		rocket = true;
	});
}
let payload_identifier = document.getElementById("payload_identifier");
if (payload_identifier != null) {
	payload_identifier.addEventListener("click", () => {
		rocket = false;
	});
}

let dummy_plot = document.getElementById("dummy_plot");
if (dummy_plot != null) {
	dummy_plot.addEventListener("click", () => {
		let nmea = document.getElementById("live_nmea").value;
		add_points(rocket, nmea);
	});
}

// GPS DATA Socket.io Listener --------------------------------------
// This will take data from the event generated by parser.py
// and feed it to the ARCGIS Rest functions below
socket.on("gps_update", function (section, nmea_sentence) {
	console.log(
		`GPS UPDATE:\nThis is from the Socket.io event listener\nRocket Section: ${section}\nFalse means Rocket, True means Payload\nOutput Received: ${nmea_sentence}`,
	);
	logger(
		`GPS UPDATE:\nThis is from the Socket.io event listener\nRocket Section: ${section}\nFalse means Rocket, True means Payload\nOutput Received: ${nmea_sentence}`,
	);
	add_points(section, nmea_sentence);
});

// Rocket

// Altitude Chart Rocket
// let altitude_chart_rocket = document.getElementById("altitude_chart_rocket");
// if (altitude_chart_rocket != null) {
// 	altitude_chart_rocket.appendChild(d3_draw_altitude_chart_rocket());
// }
// Velocity Chart Rocket
let velocity_chart_rocket = document.getElementById("velocity_chart_rocket");
if (velocity_chart_rocket != null) {
	velocity_chart_rocket.appendChild(d3_draw_velocity_chart_rocket());
}
// Acceleration Chart Rocket
let acceleration_chart_rocket = document.getElementById("acceleration_chart_rocket");
if (acceleration_chart_rocket != null) {
	acceleration_chart_rocket.appendChild(d3_draw_acceleration_chart_rocket());
}

// Payload
let altitude_chart_payload = document.getElementById("altitude_chart_payload");
if (altitude_chart_payload != null) {
	altitude_chart_payload.appendChild(d3_draw_altitude_chart_payload());
}
// Velocity Chart Rocket
let velocity_chart_payload = document.getElementById("velocity_chart_payload");
if (velocity_chart_payload != null) {
	velocity_chart_payload.appendChild(d3_draw_velocity_chart_payload());
}
// Acceleration Chart Rocket
let acceleration_chart_payload = document.getElementById("acceleration_chart_payload");
if (acceleration_chart_payload != null) {
	acceleration_chart_payload.appendChild(d3_draw_acceleration_chart_payload());
}

// document.body.appendChild(d3_draw_chart());

// --------------------------------------------------------------------------------------------------------------------
// Code that can be used with ArcGIS REST API to update and modify data values should the need arise
// const featureToAdd = {
// 	attributes: {
// 		id: 2,
// 		name: "Data Point 2",
// 	},
// 	geometry: {
// 		x: -118.807,
// 		y: 34.002,
// 		spatialReference: {
// 			wkid: 4326,
// 		},
// 	},
// };

// begin by adding a new feature to the feature service layer,
// then update its attributes,
// and finally delete it from the layer

// begin by adding a new feature to the feature service layer,
// then update its attributes,
// and finally delete it from the layer

// addFeatures({
// 	url: featureServiceLayerUrl,
// 	features: [featureToAdd],
// 	authentication,
// }).then(async (response) => {
// 	console.log(response);

// 	if (!response.addResults[0].success) {
// 		// stop early if adding a new feature was not successful
// 		return;
// 	}

// 	// perform an update to feature attributes using the objectId of the newly added feature
// 	const featureToUpdate = {
// 		attributes: {
// 			objectId: response.addResults[0].objectId,
// 			id: 2,
// 			name: "Updated Data Point 2",
// 		},
// 	};

// 	const updateResponse = await updateFeatures({
// 		url: featureServiceLayerUrl,
// 		features: [featureToUpdate],
// 		authentication,
// 	});

// 	console.log(updateResponse);

// 	if (!updateResponse.updateResults[0].success) {
// 		// stop early if updating the feature was not successful
// 		return;
// 	}

// 	// delete the feature using the objectId of the updated feature
// 	// NOTE: it is the same objectId provided in the response of `addFeatures`
// 	const featureToDelete = [updateResponse.updateResults[0].objectId];

// 	const deleteResponse = await deleteFeatures({
// 		url: featureServiceLayerUrl,
// 		objectIds: [featureToDelete],
// 		authentication,
// 	});

// 	console.log(deleteResponse);
// });
