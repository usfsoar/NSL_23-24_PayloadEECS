#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

// Create the audio shield object
AudioPlaySdWav playSdWav1;
AudioOutputI2S audioOutput;

// Create an audio connection
AudioConnection patchCord1(playSdWav1, 0, audioOutput, 0);
AudioConnection patchCord2(playSdWav1, 0, audioOutput, 1);

// SD card object
// SdFat sd = &SD;
File audioFile;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize audio library
  AudioMemory(12);

  // Initialize SD card
  if (!SD.begin(10)) {
    Serial.println("SD initialization failed!");
    while (true); // Stop here if SD card initialization failed
  }

  // Configure audio output
  audioOutput.begin();

}

void loop() {
  // Open the audio file
  audioFile = SD.open("free_bird.wav");
  if (!audioFile) {
    Serial.println("Failed to open audio file!");
    while (true); // Stop here if unable to open the audio file
  }

  // Start playing the audio file
  playSdWav1.play("free_bird.wav");

  // Wait until the audio file finishes playing
  while (playSdWav1.isPlaying()) {
    //audioOutput.analogReference(0.5);
  }

  // Close the audio file
  audioFile.close();

  // Wait for a while before playing the next audio file
  delay(5000);
}