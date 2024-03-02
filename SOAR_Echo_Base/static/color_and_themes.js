//Color and Themes-------------------------------------------------------------------------------------------------------------
//Code that will change the themes at the click of the Theme button
let theme_button = document.getElementById("theme_toggle");
theme_button.checked = false;
theme_button.addEventListener("click", () => {
	theme_changer(theme_button.checked);
	if (theme_button.checked == true) {
		theme_value = "Dark";
		// console.log(theme_value);
	} else {
		if (theme_button.checked == false) {
			theme_value = "Light";
			// console.log(theme_value);
		}
	}
});

function theme_changer(mode) {
	let logging_object = document.getElementById("live_console_holder");

	// Switching to Dark Mode
	if (mode == true) {
		// Changes the general colors
		theme_value = "Dark";
		document.getElementById("the_body").style = "background: linear-gradient(to right, #003300, black)";
		document.getElementById("left_column").style = "background: inherit; color: white";

		// Live Console -----------------------------------------------------------------------------------------------
		// Modifys the live console
		logging_object.style.border = "1px solid white";
		logging_object.style.backgroundColor = "black";

		// Modifys the live console messages
		let css_live_console_messages = document.querySelectorAll(".live_console_messages_light");
		for (let i of css_live_console_messages) {
			i.classList.remove("live_console_messages_light");
			i.classList.add("live_console_messages_dark");
		}

		// Modifys the live console error messages
		let css_live_console_error_messages_light = document.querySelectorAll(".live_console_error_messages_light");
		for (let i of css_live_console_error_messages_light) {
			i.classList.remove("live_console_error_messages_light");
			i.classList.add("live_console_error_messages_dark");
		}
		// End Live Console -------------------------------------------------------------------------------------------

		// Calls the function to change the banner colors
		banner_changer("dark");

		// Is used to tell the logger function on the page that this is the current theme
		// (JavaScript is just weird ok)
		document.getElementById("theme_value").textContent = "Dark";

		// Modifies the entire live console object to have a white border in dark theme
		document.getElementById("live_console").style.border = "";
		document.getElementById("live_console").style.borderTop = "1px solid white";
		document.getElementById("live_console").style.borderBottom = "1px solid white";
	}

	//Switching to Light mode
	if (mode == false) {
		// Changes the general colors
		theme_value = "Light";
		document.getElementById("the_body").style = "background: #CAD2D8";
		document.getElementById("left_column").style = "background: white; color: black";

		// Live Console -----------------------------------------------------------------------------------------------
		// Modifys the live console
		logging_object.style.border = "";
		logging_object.style.backgroundColor = "white";

		// Modifys the live console messages
		let css_live_console_messages = document.querySelectorAll(".live_console_messages_dark");
		for (let i of css_live_console_messages) {
			i.classList.remove("live_console_messages_dark");
			i.classList.add("live_console_messages_light");
		}

		// Modifys the live console error messages
		let css_live_console_error_messages_dark = document.querySelectorAll(".live_console_error_messages_dark");
		for (let i of css_live_console_error_messages_dark) {
			// i.style.setProperty("background-color", "rgb(253, 242, 245)");
			// i.style.setProperty("color", "red");
			i.classList.remove("live_console_error_messages_dark");
			i.classList.add("live_console_error_messages_light");
		}
		// End Live Console -------------------------------------------------------------------------------------------

		// Calls the function to change the banner colors
		banner_changer("greenwhite");

		// Is used to tell the logger function on the page that this is the current theme
		// (JavaScript is just weird ok)
		document.getElementById("theme_value").textContent = "Light";

		// Changes the live console object to have a simple border for light theme
		document.getElementById("live_console").style.border = "1px solid black";
	}
}

/*Code that will change the color of the header at will, either as response due to
clicking on the header changing buttons, or the Theme button*/
document.getElementById("header_theme_button_white").addEventListener("click", () => {
	banner_changer("greenwhite");
});
document.getElementById("header_theme_button_gold").addEventListener("click", () => {
	banner_changer("greengold");
});
document.getElementById("header_theme_button_black").addEventListener("click", () => {
	banner_changer("dark");
});
function banner_changer(banner_value) {
	let settings_gear = document.getElementById("settings_gear");
	let settings_gear_div = document.getElementById("settings_gear_div");
	header = document.getElementById("the_header");
	let settings_menu_text = document.getElementById("settings_menu_text");

	if (banner_value == "dark") {
		settings_gear_div.style.backgroundColor = "black";
		settings_gear_div.style.borderColor = "white";
		settings_menu_text.style.color = "white";

		header.style = "background: linear-gradient(to right, #003300, black)";
		settings_gear.setAttribute("class", "header_svg_color_white");
	}
	if (banner_value == "greengold") {
		settings_gear_div.style.backgroundColor = "rgb(207, 196, 147)";
		settings_gear_div.style.borderColor = "rgb(0, 103, 71)";
		settings_menu_text.style.color = "rgb(0, 103, 71)";

		header.style = "background: linear-gradient(135deg, rgba(0,103,71,1) 0%, rgba(207,196,147,1) 81%)";
		settings_gear.setAttribute("class", "header_svg_color_green");
	}
	if (banner_value == "greenwhite") {
		settings_gear_div.style.backgroundColor = "white";
		settings_gear_div.style.borderColor = "rgb(0, 103, 71)";
		settings_menu_text.style.color = "rgb(0, 103, 71)";

		header.style = "background: linear-gradient(135deg, rgba(0,103,71,1) 0%, rgba(255,255,255,1) 81%)";
		settings_gear.setAttribute("class", "header_svg_color_green");
	}
}
