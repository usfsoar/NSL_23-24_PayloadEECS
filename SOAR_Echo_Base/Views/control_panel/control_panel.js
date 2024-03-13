import '/node_modules/socket.io/client-dist/socket.io.js';
import '/node_modules/d3/dist/d3.js';
import {LinearGraph} from '/control_panel/linear_graph.js';
import {SerialMessage, LogMessage,LoraMessage} from '/_global/data_context.js'
// import { paylod_byte_parser } from '/control_panel/payload_byte_parser';

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

// Altitude Chart
let altitude_chart = document.getElementById("altitude_chart");
const alt_graph = new LinearGraph(800, 150, [0, 1000], [-5, 5])
if (altitude_chart != null) {
	altitude_chart.appendChild(alt_graph.draw());
}
//Update graph with fake data: {1000, 1}, {2000, 2}, {3000, 3}, {4000, 4}, {5000, 5}

//BACK END CODE BEGINS --------------------------------------------------------------------------------------------------------

//Script to connect with the server
var server = 'http://' + document.domain + ':' + location.port;

var socket = io.connect("http://" + document.domain + ":" + location.port);
socket.on('connect', ()=> {
    //Add to logger
    //ADd a point to the graph
    logger("Connected to the server");
});
socket.on('log_message', function(data) {
    //data is of type LogMessage
    const message = LogMessage.fromJSON(data);
    //Add it to logger as: {type}: {message}
    //If type == "error", then add it as an error message
    var error = message.type == "error";
    logger(`${message.message}`, error);
});
socket.on('serial_message', function(data) {
    console.log(data);
    //data is of type SerialMessage
    const message = SerialMessage.fromJSON(data);
    //Add it to logger as: {sender}: {message}
    logger(`${message.device}: ${message.message}`);
});
/**
|Command|Definition|Response|Definition|
|---|---|---|---|
|`PI`|Ping|`PO{T-time}`|Pong with time|
|`AS`|Altitude Single|`AS{T-time}{Altitude-float}`|Altitude Single Data with time|
|`AR`|Altitude Repeat|`AR{T-time}`|Altitude Repeat Received with time|
|`AW`|Write altitude thresholds|`AW{T-time}`|Thresholds written with time|
|||`AF{T-time}`|Failed to set thresholds with time|
|`AT`|Get altitude thresholds|`AT{T-time}{H1-float}{H2-float}{H3-float}`|Thresholds data with time|
|`DP`|Deploy|`DP{T-time}`|Deploy Received with time|
|`DS`|Deploy Status|`DS{T-time}{Status-uint8_t}`|Deploy Status Data with time|
|`DR`|Deploy Status Repeat|`DR{T-time}`|Deploy Status Repeat Received with time|
|`DT`|Deploy Stop|`DT{T-time}`|Deploy Stop Received with time|
|`DR`|Deploy Reset|`DR{T-time}`|Deploy Reset Received with time|
|`DC`|Deploy Retract|`DC{T-time}``|Deploy Retract Received with time|
|`LI`|Distance sensor|`LI{T-time}{Distance-uint16_t}`|Distance sensor data with time|
|`LR`|Distance sensor repeat|`LR{T-time}`|Distance sensor repeat received with time|
|`IS`|All info single|`IS{T-time}{Altitude-float}{Distance-uint16_t}{Status-uint8_t}`|All info data with time|
|`IR`|All info repeat|`IR{T-time}`|All info repeat received with time|
|`JF`|Jog Forward|`JF{T-time}`|Jog Forward Received with time|
|`JR`|Jog Reverse|`JR{T-time}`|Jog Reverse Received with time|
|`RS`|Stop any repeating data|`RS{T-time}`|Stop any repeating data received with time|
|`NH`|Not handled (n bytes)|`NH{T-time}{Command-nB}`|Not handled command with time|
 */
socket.on('lora_message', (data)=> {
    console.log(data);
    const loraMsg = LoraMessage.fromJSON(data);
    //Add it to logger as: {sender}: {message}
    logger(`${loraMsg.device}: ${loraMsg.packet.toStr()}`);
    if (loraMsg.device === "Payload" && loraMsg.packet.valid_data) {
        const byte_data = loraMsg.packet.data_bytes;
        const command = String.fromCharCode(...byte_data.slice(0, 2));
        //Get the last four bytes as a long int (uint32_t)
        const lastFourBytes = new Uint8Array(byte_data.slice(-4));
        const time = new Uint32Array(lastFourBytes.buffer)[0];

        if(command === "PO"){
            console.log("Pong with time: " + time);
        }
        else if(command === "WU"){
            console.log("Wake Up Received with time: " + time);
        }
        else if(command === "AS"){
            //First 2 bytes are the command, next 4 bytes are the altitude
            const altitude_bytes = new Uint8Array(byte_data.slice(2, 6));
            const altitude = new Float32Array(altitude_bytes.buffer)[0];
            alt_graph.updateChart({time: time, value: altitude});
            console.log("Altitude Single Data with time: " + time + " Altitude: " + altitude);
        }
        else if(command === "AR"){
            console.log("Altitude Repeat Received with time: " + time);

        }
        else if(command === "AW"){
            console.log("Thresholds written with time: " + time);
        }
        else if(command === "AF"){
            console.log("Failed to set thresholds with time: " + time);
        }
        else if(command === "AT"){
            const h1_bytes = new Uint8Array(byte_data.slice(2, 6));
            const h2_bytes = new Uint8Array(byte_data.slice(6, 10));
            const h3_bytes = new Uint8Array(byte_data.slice(10, 14));
            const h1 = new Float32Array(h1_bytes.buffer);
            const h2 = new Float32Array(h2_bytes.buffer);
            const h3 = new Float32Array(h3_bytes.buffer);
            console.log("Thresholds data with time: " + time + " H1: " + h1 + " H2: " + h2 + " H3: " + h3);
        }
        else if(command === "DP"){
            console.log("Deploy Received with time: " + time);
        }
        else if(command === "DS"){
            const status_bytes = new Uint8Array(byte_data.slice(2, 3));
            const status = new Uint8Array(status_bytes.buffer);
            console.log("Deploy Status Data with time: " + time + " Status: " + status);
        }
        else if(command === "DR"){
            console.log("Deploy Status Repeat Received with time: " + time);
        }
        else if(command === "DT"){
            console.log("Deploy Stop Received with time: " + time);
        }
        else if(command === "DR"){
            console.log("Deploy Reset Received with time: " + time);
        }
        else if(command === "DC"){
            console.log("Deploy Retract Received with time: " + time);
        }
        else if(command === "LI"){
            const distance_bytes = new Uint8Array(byte_data.slice(2, 4));
            const distance = new Uint16Array(distance_bytes.buffer);
            console.log("Distance sensor data with time: " + time + " Distance: " + distance);
        }
        else if(command === "LR"){
            console.log("Distance sensor repeat received with time: " + time);
        }
        else if(command === "IS"){
            const altitude_bytes = new Uint8Array(byte_data.slice(2, 6));
            const distance_bytes = new Uint8Array(byte_data.slice(6, 8));
            const status_bytes = new Uint8Array(byte_data.slice(8, 9));
            const altitude = new Float32Array(altitude_bytes.buffer);
            const distance = new Uint16Array(distance_bytes.buffer);
            const status = new Uint8Array(status_bytes.buffer);
            console.log("All info data with time: " + time + " Altitude: " + altitude + " Distance: " + distance + " Status: " + status);
        }
        else if(command === "IR"){
            console.log("All info repeat received with time: " + time);
        }
        else if(command === "JF"){
            console.log("Jog Forward Received with time: " + time);
        }
        else if(command === "JR"){
            console.log("Jog Reverse Received with time: " + time);
        }
        else if(command === "RS"){
            console.log("Stop any repeating data received with time: " + time);
        }
        else if(command === "NH"){
            const command_bytes = new Uint8Array(byte_data.slice(2, byte_data.length));
            const command_str = String.fromCharCode(...command_bytes);
            console.log("Not handled command with time: " + time + " Command: " + command_str);
        }

    }
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
	const fetch_promise = fetch(server+'/close_serial/payload');
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
	const fetch_promise = fetch(server+'/close_serial/recovery');
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

	const parsedCommand = parseString(message);
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

const sendToPayload = (command, id) =>{
    const parsedCommand = parseString(command);
    const json_message = {
        "device_name": "payload",
        "destination_address": parseInt(id),
        "command": parsedCommand
    }
    const fetch_promise = fetch(server + '/send_command', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(json_message)
    });
}
const sendToRecovery = (command, id) =>{
    const parsedCommand = parseString(command);
    const json_message = {
        "device_name": "recovery",
        "destination_address": parseInt(id),
        "command": parsedCommand
    }
    const fetch_promise = fetch(server + '/send_command', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(json_message)
    });
}


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
    document.getElementById("rpt_stop_btn").removeAttribute("disabled");
})

// DEPLOY, RETRACT, STOP, RESET, STATUS, ALTITUDE,
document.getElementById("ping_recovery_btn").addEventListener("click", ()=>{
    SendCommand("PING", Recovery_ID);
});
document.getElementById("ping_payload_btn").addEventListener("click", ()=>{
	sendToPayload("PI;", deployment_id);
});
document.getElementById("deploy_btn").addEventListener("click", ()=>{
    sendToPayload("DP;", deployment_id);
});
document.getElementById("retract_btn").addEventListener("click", ()=>{
    sendToPayload("DC;", deployment_id);
});
document.getElementById("stop_btn").addEventListener("click", ()=>{
    sendToPayload("DT;", deployment_id);
});
document.getElementById("reset_btn").addEventListener("click", ()=>{
    sendToPayload("DR;", deployment_id);
});
document.getElementById("status_btn").addEventListener("click", ()=>{
    sendToPayload("DS;", deployment_id);
});
document.getElementById("altitude_btn").addEventListener("click", ()=>{
    sendToPayload("AS;", deployment_id);
});
document.getElementById("gps_btn").addEventListener("click", ()=>{
	SendCommand("GPS", Recovery_ID);
})
document.getElementById("gps_btn_rpt").addEventListener("click", ()=>{
	SendCommand("GPS:repeat", Recovery_ID);
})
document.getElementById("altitude_btn_rpt").addEventListener("click", ()=>{
    sendToPayload("AR;", deployment_id);
});
document.getElementById("status_btn_rpt").addEventListener("click", ()=>{
    sendToPayload("DR;", deployment_id);
});
document.getElementById("deploy_btn_rtp").addEventListener("click", ()=>{
    sendToPayload("DR;", deployment_id);
})
document.getElementById("deploy_btn_dist").addEventListener("click", ()=>{
    sendToPayload("LI;", deployment_id);
})
document.getElementById("jog_fwd_btn").addEventListener("click", ()=>{
    sendToPayload("JF;", deployment_id);
})
document.getElementById("jog_rev_btn").addEventListener("click", ()=>{
    sendToPayload("JR;", deployment_id);
})
document.getElementById("rpt_stop_btn").addEventListener("click", ()=>{
    sendToPayload("RS;", deployment_id);
});

function d3_draw_altitude_chart() {
	// Declare the chart dimensions and margins.
    const width = 800;
    const height = 150;
    const marginTop = 20;
    const marginRight = 20;
    const marginBottom = 30;
    const marginLeft = 40;

    // Declare the x (horizontal position) scale as a linear scale.
    // For example, let's assume the time range is from 0 to 180000 milliseconds (3 minutes).
    const x = d3
        .scaleLinear()
        .domain([0, 180000]) // Adjust this domain based on your actual time range in milliseconds.
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

function add_altitude_data(newData) {
    // Bind the new data to the circles.
    const circles = svg.selectAll("circle").data(newData);

    // Enter new circles.
    circles.enter()
        .append("circle")
        .attr("cx", d => x(d.time))
        .attr("cy", d => y(d.value))
        .attr("r", 5)
        .style("fill", "steelblue");

    // Update existing circles.
    circles
        .attr("cx", d => x(d.time))
        .attr("cy", d => y(d.value));

    // Remove old circles.
    circles.exit().remove();
}
// function add_altitude_data() {
// 	const altitudeData = [
// 		{time: 0, altitude: 0},
// 		{time: 5, altitude: 100},
// 		{time: 10, altitude: 200},
// 		{time: 15, altitude: 150},
// 		{time: 20, altitude: 300},
// 		{time: 30, altitude: 700},
// 		// Add more data points as needed
// 	];
// }

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