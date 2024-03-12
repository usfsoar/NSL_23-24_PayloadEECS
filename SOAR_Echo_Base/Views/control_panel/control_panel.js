import '/node_modules/socket.io/client-dist/socket.io.js';
import '/node_modules/d3/dist/d3.min.js';

//FRONT END CODE BEGINS -------------------------------------------------------------------------------------------------------
// Code that logs information passed to it to the Console on screen
let logging_object = document.getElementById("live_console");
let logger_reference_node = document.createTextNode("");
logging_object.appendChild(logger_reference_node);
function logger(information, error=false) {
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

	if(error == true) {
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


//BAROMETER Frontend Class
class Barometer {
	constructor(pressure=NULL, temperature=NULL, altitude=NULL){
		this.pressure=pressure;
		this.temperature=temperature;
		this.altitude=altitude;
	}
}

//IMU Frontend Class
class IMU {
	constructor(acceleration_x=NULL, acceleration_y=NULL, acceleration_z=NULL,
              gyro_x=NULL, gyro_y=NULL, gyro_z=NULL, magnetic_x=NULL, magnetic_y=NULL, magnetic_z=NULL){
		this.acceleration_x = acceleration_x
		this.acceleration_y = acceleration_y
		this.acceleration_z = acceleration_z
		this.gyro_x = gyro_x
		this.gyro_y = gyro_y
		this.gyro_z = gyro_z
		this.magnetic_x = magnetic_x
		this.magnetic_y = magnetic_y
		this.magnetic_z = magnetic_z
	}
}




//BACK END CODE BEGINS --------------------------------------------------------------------------------------------------------

//Script to connect with the server
var server = 'http://' + document.domain + ':' + location.port;

var socket = io.connect("http://" + document.domain + ":" + location.port);
socket.on('connect', function() {
    //Add to logger
    logger("Connected to the server");
});

// Checks if an entered COM port is valid or not
function isValidComPort(comPort) {
	var pattern = /^COM\d+$/;
	return pattern.test(comPort);
	}


// NOTE: THIS ONLY CREATES THE FRONT END. THE BACK END, AS OF 2:44PM 2/4/2024 IS NOT COMPLETE
// Functions need to be made in control_panel.py to handle each request to the back end to open or close, each serial port
// Initiates connection to the back end for the Payload System
document.getElementById("telemetry_start").addEventListener("click", () => {
	console.log("Beginning Telemetry");
	let com_port = document.getElementById("com_port").value;
	if (isValidComPort(com_port) == false) {
		console.error(`Invalid COM Port: ${com_port}`);
		logger(`Invalid COM Port: ${com_port}`, true);
		document.getElementById("invalid_com_port_notif").style.display = "block";
		return
	}
	if (isValidComPort(com_port) == true) {
		logger(`Valid COM Port: ${com_port}`);
		document.getElementById("invalid_com_port_notif").style.display = "none";
	}
	const fetch_promise = fetch(server+'/start_payload_serial/'+com_port);
    fetch_promise
    .then((response)=>{
        if(!response.ok){
            throw new Error(`HTTP error: ${response.status}`);
        }
        return response.text();
    })
    .then((text)=>{
        console.log(text);
		logger(text);
    });
});

// Initiates connection to the back end for the GPS System
document.getElementById("gps_connection_start").addEventListener("click", () => {
	console.log("Beginning GPS Connection");
	let com_port = document.getElementById("com_port").value;
	if (isValidComPort(com_port) == false) {
		console.error(`Invalid COM Port: ${com_port}`);
		logger(`Invalid COM Port: ${com_port}`, true);
		document.getElementById("invalid_com_port_notif").style.display = "block";
		return
	}
	if (isValidComPort(com_port) == true) {
		logger(`Valid COM Port: ${com_port}`);
		document.getElementById("invalid_com_port_notif").style.display = "none";
	}
	const fetch_promise = fetch(server+'/serial_start/'+com_port);
    fetch_promise
    .then((response)=>{
        if(!response.ok){
            throw new Error(`HTTP error: ${response.status}`);
        }
        return response.text();
    })
    .then((text)=>{
        console.log(text);
		logger(text);
    });
});

// Close the Payload serial connection
document.getElementById("close_serial").addEventListener("click", () => {
	const fetch_promise = fetch(server+'/close_serial');
	fetch_promise
	.then((response)=>{
        if(!response.ok){
            throw new Error(`HTTP error: ${response.status}`);
        }
        return response.text();
    })
    .then((text)=>{
        console.log(text);
		logger(text);
    });
});

// Close the GPS serial connection
document.getElementById("gps_connection_close").addEventListener("click", () => {
	const fetch_promise = fetch(server+'/close_serial');
	fetch_promise
	.then((response)=>{
        if(!response.ok){
            throw new Error(`HTTP error: ${response.status}`);
        }
        return response.text();
    })
    .then((text)=>{
        console.log(text);
		logger(text);
    });
});

// Sends commands to the Serial Console in the back end
function SendCommand(command, id = 0){
	var full_command = `${command},${id}`
	const fetch_promise = fetch(server + '/command/'+full_command);
	fetch_promise
	.then((response)=>{
		if(!response.ok){
            throw new Error(`HTTP error: ${response.status}`);
			logger(`HTTP error: ${response.status}`);
        }
        return response.text();
	})
	.then((text)=>{
		console.log(text);
		logger(text);
	})
	.catch((err)=>{
		logger(err, true);
		console.log(err);
	})
}
function parseString(input) {
    // Split the input into format string and values
    const [formatString, valuesString] = input.split(';');
    // Split the values into an array
    const values = valuesString.split(',');
    // Regular expression to match format specifiers
    const formatSpecifierRegex = /%[df]/g;

    // Array to store the results
    let result = [];

    // Index to track the current position in the values array
    let valueIndex = 0;

    // Split the format string by format specifiers
    const parts = formatString.split(formatSpecifierRegex);

    // Iterate over the parts of the format string
    for (let i = 0; i < parts.length; i++) {
        // Add the string part to the result
        if (parts[i]) {
            result.push({ data: parts[i], type: 'string' });
        }
	
        // Add the value part to the result, if there is a corresponding value
        if (valueIndex < values.length) {
            const specifier = formatString.match(formatSpecifierRegex)?.[i];
            if (specifier === '%d') {
                result.push({ data: parseInt(values[valueIndex]), type: 'int' });
            } else if (specifier === '%f') {
                result.push({ data: parseFloat(values[valueIndex]), type: 'float' });
            }
            valueIndex++;
        }
    }
			
    return result;
}

document.getElementById("live_console_send_msg_btn").addEventListener("click", () => {
	let message = document.getElementById("live_console_send_msg_field").value;
	let id = document.getElementById("live_console_send_msg_id").value;

	//Must convert the message into the following json:
	/* {
		#   "device_name": "payload" or "recovery",
		#   "destination_address": int,
		#   "command": [{data, type}, {data, type}, ...]
		# }
		# The data will be of different types
	})*/
	//message will be parsed like how C injects data into a string using:
	// %d: int and boolean, %f: float
	//For example a message that sends ATWR%d%f;1,2.55
	//Should be parsed as: [{"data":"ATWR", "type":"string"}, {"data":1, "type":"int"}, {"data":2.55, "type":"float"}]

	parsedCommand = parseString(message);
	var json_message = {
		"device_name": "payload",
		"destination_address": parseInt(id),
		"command": parsedCommand
	}
	//Make an http post request to /send_command
	const fetch_promise = fetch(server + '/send_command', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(json_message)
	});
});
const deployment_id = 5;
const Recovery_ID = 7;

// For Dev Purposes, this will enable all of the buttons
document.getElementById("enable_all").addEventListener("click", () => {
	document.getElementById("deploy_btn").removeAttribute("disabled");
	document.getElementById("ping_recovery_btn").removeAttribute("disabled");
	document.getElementById("ping_payload_btn").removeAttribute("disabled");
	document.getElementById("deploy_btn").removeAttribute("disabled");
	document.getElementById("retract_btn").removeAttribute("disabled");
	document.getElementById("stop_btn").removeAttribute("disabled");
	document.getElementById("reset_btn").removeAttribute("disabled");
	document.getElementById("status_btn").removeAttribute("disabled");
	document.getElementById("altitude_btn").removeAttribute("disabled");
	document.getElementById("gps_btn").removeAttribute("disabled");
	document.getElementById("gps_btn_rpt").removeAttribute("disabled");
	document.getElementById("altitude_btn_rpt").removeAttribute("disabled");
	document.getElementById("status_btn_rpt").removeAttribute("disabled");
	document.getElementById("deploy_btn_rtp").removeAttribute("disabled");
	document.getElementById("deploy_btn_dist").removeAttribute("disabled");
	document.getElementById("jog_fwd_btn").removeAttribute("disabled");
	document.getElementById("jog_rev_btn").removeAttribute("disabled");
})

// DEPLOY, RETRACT, STOP, RESET, STATUS, ALTITUDE,
document.getElementById("ping_recovery_btn").addEventListener("click", ()=>{
	SendCommand("PING", Recovery_ID);
});
document.getElementById("ping_payload_btn").addEventListener("click", ()=>{
	SendCommand("PING", deployment_id);
});
document.getElementById("deploy_btn").addEventListener("click", ()=>{
	SendCommand("DEPLOY", deployment_id);
});
document.getElementById("retract_btn").addEventListener("click", ()=>{
	SendCommand("RETRACT", deployment_id);
});
document.getElementById("stop_btn").addEventListener("click", ()=>{
	SendCommand("STOP", deployment_id);
});
document.getElementById("reset_btn").addEventListener("click", ()=>{
	SendCommand("RESET", deployment_id);
});
document.getElementById("status_btn").addEventListener("click", ()=>{
	SendCommand("STATUS", deployment_id);
});
document.getElementById("altitude_btn").addEventListener("click", ()=>{
	SendCommand("ALTITUDE", deployment_id);
});
document.getElementById("gps_btn").addEventListener("click", ()=>{
	SendCommand("GPS", Recovery_ID);
})
document.getElementById("gps_btn_rpt").addEventListener("click", ()=>{
	SendCommand("GPS:repeat", Recovery_ID);
})
document.getElementById("altitude_btn_rpt").addEventListener("click", ()=>{
	SendCommand("ALTITUDE:repeat", deployment_id)
});
document.getElementById("status_btn_rpt").addEventListener("click", ()=>{
	SendCommand("STATUS:repeat", deployment_id)
});
document.getElementById("deploy_btn_rtp").addEventListener("click", ()=>{
	SendCommand("DEPLOY:repeat", deployment_id)
})
document.getElementById("deploy_btn_dist").addEventListener("click", ()=>{
	SendCommand("DISTANCE", deployment_id)
})
document.getElementById("jog_fwd_btn").addEventListener("click", ()=>{
	SendCommand("JOG:FWD", deployment_id)
})
document.getElementById("jog_rev_btn").addEventListener("click", ()=>{
	SendCommand("JOG:REV", deployment_id)
})

function d3_draw_altitude_chart() {
	// let hi = false;
	// const altitudeData = [
	// 	{time: 0, altitude: 0},
	// 	{time: 5, altitude: 100},
	// 	{time: 10, altitude: 200},
	// 	{time: 15, altitude: 150},
	// 	{time: 20, altitude: 300},
	// 	{time: 30, altitude: 700},
	// 	// Add more data points as needed
	// ];
	// // import * as d3 from "https://cdn.jsdelivr.net/npm/d3@7/+esm";
	// // Sample altitude data for demonstration

	// // Set up chart dimensions
	// const width = 600;
	// const height = 400;
	// const margin = {top: 20, right: 20, bottom: 30, left: 50};

	// // Create SVG container
	// let svg = d3
	// 	.select("body")
	// 	.append("svg")
	// 	.attr("width", width + margin.left + margin.right)
	// 	.attr("height", height + margin.top + margin.bottom)
	// 	.append("g")
	// 	.attr("transform", `translate(${margin.left},${margin.top})`);

	// // Define scales
	// let yScale = d3
	// 	.scaleLinear()
	// 	.domain([0, d3.max(altitudeData, (d) => d.altitude)])
	// 	.range([height, 0]);

	// // Define line function
	// let line = d3
	// 	.line()
	// 	.x((d) => xScale(d.time))
	// 	.y((d) => yScale(d.altitude));

	// // Add line to the chart
	// svg
	// 	.append("path")
	// 	.datum(altitudeData)
	// 	.attr("fill", "none")
	// 	.attr("stroke", "steelblue")
	// 	.attr("stroke-width", 2)
	// 	.attr("d", line);

	// // Add x-axis
	// svg.append("g").attr("transform", `translate(0,${height})`).attr("class", "x-axis").call(d3.axisBottom(xScale));

	// // Add y-axis
	// svg.append("g").attr("class", "y-axis").call(d3.axisLeft(yScale));

	// // Add tooltips
	// let tooltip = svg.append("g").attr("class", "tooltip").style("display", "none");

	// tooltip
	// 	.append("rect")
	// 	.attr("width", 60)
	// 	.attr("height", 20)
	// 	.attr("fill", "white")
	// 	.style("stroke", "steelblue")
	// 	.style("stroke-width", 2);

	// let tooltipText = tooltip
	// 	.append("text")
	// 	.attr("x", 30)
	// 	.attr("y", 10)
	// 	.attr("text-anchor", "middle")
	// 	.attr("dy", "0.35em")
	// 	.style("fill", "steelblue")
	// 	.style("font-size", "12px");

	// // Add event listeners for tooltips
	// svg
	// 	.selectAll("circle")
	// 	.data(altitudeData)
	// 	.enter()
	// 	.append("circle")
	// 	.attr("cx", (d) => xScale(d.time))
	// 	.attr("cy", (d) => yScale(d.altitude))
	// 	.attr("r", 5)
	// 	.style("fill", "steelblue")
	// 	.on("mouseover", function (event, d) {
	// 		tooltip.style("display", null);
	// 		tooltip.attr("transform", `translate(${xScale(d.time)},${yScale(d.altitude)})`);
	// 		tooltipText.text(d.altitude);
	// 	})
	// 	.on("mouseout", function () {
	// 		tooltip.style("display", "none");
	// 	});

	// // document.getElementById("hi").addEventListener("click", () => {
	// if ((hi = true)) {
	// 	altitudeData.push({time: 35, altitude: 900});
	// 	console.log(altitudeData);
	// 	// Update scales with the new data range
	// 	// xScale.domain([0, d3.max(altitudeData, d => d.time)]);
	// 	yScale.domain([0, d3.max(altitudeData, (d) => d.altitude)]);

	// 	// Select the existing line and update the data
	// 	svg
	// 		.select("path")
	// 		.datum(altitudeData)
	// 		.transition()
	// 		.duration(250) // Set a transition duration for a smooth update
	// 		.attr("d", line);

	// 	// // Update the x-axis
	// 	// svg.select('.x-axis')
	// 	// .transition()
	// 	// .duration(250)
	// 	// .call(d3.axisBottom(xScale));

	// 	// Update the y-axis
	// 	svg.select(".y-axis").transition().duration(250).call(d3.axisLeft(yScale));

	// 	// Select existing circles and bind new data
	// 	const circles = svg.selectAll("circle").data(altitudeData);

	// 	// Update existing circles
	// 	circles
	// 		.transition()
	// 		.duration(250)
	// 		.attr("cx", (d) => xScale(d.time))
	// 		.attr("cy", (d) => yScale(d.altitude));

	// 	// Add new circles
	// 	circles
	// 		.enter()
	// 		.append("circle")
	// 		.attr("cx", (d) => xScale(d.time))
	// 		.attr("cy", (d) => yScale(d.altitude))
	// 		.attr("r", 5)
	// 		.style("fill", "steelblue")
	// 		.on("mouseover", function (event, d) {
	// 			tooltip.style("display", null);
	// 			tooltip.attr("transform", `translate(${xScale(d.time)},${yScale(d.altitude)})`);
	// 			tooltipText.text(d.altitude);
	// 		})
	// 		.on("mouseout", function () {
	// 			tooltip.style("display", "none");
	// 		});

	// 	// Remove any circles not present in the new data
	// 	circles.exit().remove();
	// }
	// // Append the SVG element.
	// let holder = document.createElement("div");
	// holder.append(svg.node());
	// return holder;

	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	// const x = d3
	// 	.scaleUtc()
	// 	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
	// 	.range([marginLeft, width - marginRight]);
	const x = d3
		.scaleTime([new Date(2000, 0, 1, 8, 0, 0, 0), new Date(2000, 0, 1, 8, 3, 0, 0)], [0, 960])
		.range([marginLeft, width - marginRight]);
	x(new Date(2000, 0, 1, 5)); // 200
	x(new Date(2000, 0, 1, 16)); // 640
	x.invert(200); // Sat Jan 01 2000 05:00:00 GMT-0800 (PST)
	x.invert(640); // Sat Jan 01 2000 16:00:00 GMT-0800 (PST)
	x.tickFormat("%S");
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

function d3_draw_pressure_chart() {
	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	// const x = d3
	// 	.scaleUtc()
	// 	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
	// 	.range([marginLeft, width - marginRight]);
	const x = d3
		.scaleTime([new Date(2000, 0, 1, 8, 0, 0, 0), new Date(2000, 0, 1, 8, 3, 0, 0)], [0, 960])
		.range([marginLeft, width - marginRight]);
	x(new Date(2000, 0, 1, 5)); // 200
	x(new Date(2000, 0, 1, 16)); // 640
	x.invert(200); // Sat Jan 01 2000 05:00:00 GMT-0800 (PST)
	x.invert(640); // Sat Jan 01 2000 16:00:00 GMT-0800 (PST)
	x.tickFormat("%S");
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

function add_altitude_data() {
	const altitudeData = [
		{time: 0, altitude: 0},
		{time: 5, altitude: 100},
		{time: 10, altitude: 200},
		{time: 15, altitude: 150},
		{time: 20, altitude: 300},
		{time: 30, altitude: 700},
		// Add more data points as needed
	];
}

// document.getElementById("data_add").addEventListener("click", () => {
// 	add_altitude_data();
// });

function d3_draw_acceleration_chart() {
	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
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
function d3_draw_gyroscope_chart() {
	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	// const x = d3
	// 	.scaleUtc()
	// 	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
	// 	.range([marginLeft, width - marginRight]);
	const x = d3
		.scaleTime([new Date(2000, 0, 1, 8, 0, 0, 0), new Date(2000, 0, 1, 8, 3, 0, 0)], [0, 960])
		.range([marginLeft, width - marginRight]);
	x(new Date(2000, 0, 1, 5)); // 200
	x(new Date(2000, 0, 1, 16)); // 640
	x.invert(200); // Sat Jan 01 2000 05:00:00 GMT-0800 (PST)
	x.invert(640); // Sat Jan 01 2000 16:00:00 GMT-0800 (PST)
	x.tickFormat("%S");
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
function d3_draw_magnetometer_chart() {
	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	// const x = d3
	// 	.scaleUtc()
	// 	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
	// 	.range([marginLeft, width - marginRight]);
	const x = d3
		.scaleTime([new Date(2000, 0, 1, 8, 0, 0, 0), new Date(2000, 0, 1, 8, 3, 0, 0)], [0, 960])
		.range([marginLeft, width - marginRight]);
	x(new Date(2000, 0, 1, 5)); // 200
	x(new Date(2000, 0, 1, 16)); // 640
	x.invert(200); // Sat Jan 01 2000 05:00:00 GMT-0800 (PST)
	x.invert(640); // Sat Jan 01 2000 16:00:00 GMT-0800 (PST)
	x.tickFormat("%S");
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
function d3_draw_decibel_chart() {
	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	// const x = d3
	// 	.scaleUtc()
	// 	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
	// 	.range([marginLeft, width - marginRight]);
	const x = d3
		.scaleTime([new Date(2000, 0, 1, 8, 0, 0, 0), new Date(2000, 0, 1, 8, 3, 0, 0)], [0, 960])
		.range([marginLeft, width - marginRight]);
	x(new Date(2000, 0, 1, 5)); // 200
	x(new Date(2000, 0, 1, 16)); // 640
	x.invert(200); // Sat Jan 01 2000 05:00:00 GMT-0800 (PST)
	x.invert(640); // Sat Jan 01 2000 16:00:00 GMT-0800 (PST)
	x.tickFormat("%S");
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
function d3_draw_g_forces_chart() {
	// Declare the chart dimensions and margins.
	const width = 800;
	const height = 150;
	const marginTop = 20;
	const marginRight = 20;
	const marginBottom = 30;
	const marginLeft = 40;

	// Declare the x (horizontal position) scale.
	// const x = d3
	// 	.scaleUtc()
	// 	.domain([new Date("2023-01-01"), new Date("2024-01-01")])
	// 	.range([marginLeft, width - marginRight]);
	const x = d3
		.scaleTime([new Date(2000, 0, 1, 8, 0, 0, 0), new Date(2000, 0, 1, 8, 3, 0, 0)], [0, 960])
		.range([marginLeft, width - marginRight]);
	x(new Date(2000, 0, 1, 5)); // 200
	x(new Date(2000, 0, 1, 16)); // 640
	x.invert(200); // Sat Jan 01 2000 05:00:00 GMT-0800 (PST)
	x.invert(640); // Sat Jan 01 2000 16:00:00 GMT-0800 (PST)
	x.tickFormat("%S");
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

// Altitude Chart
let altitude_chart = document.getElementById("altitude_chart");
if (altitude_chart != null) {
	altitude_chart.appendChild(d3_draw_altitude_chart());
}
// Pressure Chart
let pressure_chart = document.getElementById("pressure_chart");
if (pressure_chart != null) {
	pressure_chart.appendChild(d3_draw_pressure_chart());
}
// Acceleration Chart
let acceleration_chart = document.getElementById("acceleration_chart");
if (acceleration_chart != null) {
	acceleration_chart.appendChild(d3_draw_acceleration_chart());
}
// Gyroscope Chart
let gyroscope_chart = document.getElementById("gyroscope_chart");
if (gyroscope_chart != null) {
	gyroscope_chart.appendChild(d3_draw_gyroscope_chart());
}
// Magnetometer Chart
let magnetometer_chart = document.getElementById("magnetometer_chart");
if (magnetometer_chart != null) {
	magnetometer_chart.appendChild(d3_draw_magnetometer_chart());
}
// Decibel Chart
let decibel_chart = document.getElementById("decibel_chart");
if (decibel_chart != null) {
	decibel_chart.appendChild(d3_draw_decibel_chart());
}
// G-Forces Chart
let g_forces_chart = document.getElementById("g_forces_chart");
if (g_forces_chart != null) {
	g_forces_chart.appendChild(d3_draw_g_forces_chart());
}