
//----------------------------------
#include <Audio.h>
#include <Wire.h>
#include "SOAR_SD_CARD.h"

// Create the audio shield object


// Create an audio test tone, 440 Hz sine wave
AudioSynthWaveformSine sine1;
AudioOutputI2S2 audioOutput;  // Use AudioOutputI2S2 for secondary I2S pins

// Connect the sine wave output to the I2S output
AudioConnection patchCord1(sine1, 0, audioOutput, 0);
AudioConnection patchCord2(sine1, 0, audioOutput, 1);

SOAR_SD_CARD sd_card(10);

void setup() {
  // Enable the audio output
  AudioMemory(12);
  sine1.frequency(440);
  sine1.amplitude(0.4);
}

void loop() {
  /// Set the sine wave frequency
  int speaker_pin = 7;
  tone(speaker_pin, 440, 500);
  delay(1000);
  tone(speaker_pin, 880, 500);
  delay(1000);
}
