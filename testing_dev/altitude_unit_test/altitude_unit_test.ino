#define ALT_TRSH_CHECK 850
#define LOW_ALT_TRSH_CHECK 300

// Simulated altitude data
float altitudeData[] = {500, 700, 900, 950, 800, 750, 600, 400, 200, 100, 300, 350};

void setup() {
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < sizeof(altitudeData) / sizeof(altitudeData[0]); i++) {
    int trigger = altitudeTrigger(altitudeData[i]);
    Serial.print("Altitude: ");
    Serial.print(altitudeData[i]);
    Serial.print(", Trigger: ");
    Serial.println(trigger);
    delay(1000); // Adjust delay as needed
  }
}

float previous_altitude = -300;
float immediate_previous = -6000;

int altitudeTrigger(float current_altitude) {
  int res = 0;

  // Check if the altitude is decreasing and above ALT_TRSH_CHECK
  if ((current_altitude > ALT_TRSH_CHECK) && (current_altitude - previous_altitude < -2)) {
    res = 1;
  }

  if (current_altitude > previous_altitude) {
    if (current_altitude - immediate_previous < 800 || immediate_previous == -60000) {
      // Default value for errors
      previous_altitude = current_altitude;
    }
  }

  if (current_altitude - immediate_previous > 800) {
    // If altitude shows sudden changes it must be a glitch
    res = 0;
  }

  // Update previous_altitude for the next function call
  immediate_previous = current_altitude;
  return res;
}
