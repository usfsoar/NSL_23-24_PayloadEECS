import * as d3 from "d3";
// import * as hi from "control_panel.html";
// export {d3_draw_chart};

import * as io from "/static/socket.io.js";

//WEBSOCKET CODE
var socket = io.connect("http://" + document.domain + ":" + location.port);

socket.on("gps_update", function (data) {
	console.log("GPS UPDATE", data);
	logger("GPS UPDATE", data);
	// console.log(data.nmea);
	// plot_varn(data.nmea);
	// plot_usf(data.nmea);

	if (active_map == "USF") {
		plot_usf(data.nmea);
	}
	if (active_map == "Varn") {
		plot_varn(data.nmea);
	}
});

socket.on("gps_message", function (data) {
	console.log(data);
	logger(data);
});

function d3_draw_altitude_chart_rocket() {
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
// Rocket
document.getElementById("altitude_chart_rocket").appendChild(d3_draw_altitude_chart_rocket());
document.getElementById("velocity_chart_rocket").appendChild(d3_draw_velocity_chart_rocket());
document.getElementById("acceleration_chart_rocket").appendChild(d3_draw_acceleration_chart_rocket());

// Payload
document.getElementById("altitude_chart_payload").appendChild(d3_draw_altitude_chart_payload());
document.getElementById("velocity_chart_payload").appendChild(d3_draw_velocity_chart_payload());
document.getElementById("acceleration_chart_payload").appendChild(d3_draw_acceleration_chart_payload());
// document.body.appendChild(d3_draw_chart());
