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
	let css_live_console_error_messages = document.getElementsByClassName("live_console_error_messages");
	if (mode == true) {
		//Switching to dark mode
		theme_value = "Dark";
		document.getElementById("the_body").style = "background: linear-gradient(to right, #003300, black)";
		document.getElementById("left_column").style = "background: inherit; color: white";
		logging_object.style.borderColor = "white";
		for (let i of css_live_console_error_messages) {
			i.style.setProperty("background-color", "rgb(75, 47, 54)");
			i.style.setProperty("color", "rgb(255, 179, 210)");
		}
		banner_changer("dark");
	}
	if (mode == false) {
		//Switching to light mode
		theme_value = "Light";
		document.getElementById("the_body").style = "background: #CAD2D8";
		document.getElementById("left_column").style = "background: white; color: black";
		logging_object.style.borderColor = "black";
		for (let i of css_live_console_error_messages) {
			i.style.setProperty("background-color", "rgb(253, 242, 245)");
			i.style.setProperty("color", "red");
		}
		banner_changer("greenwhite");
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
