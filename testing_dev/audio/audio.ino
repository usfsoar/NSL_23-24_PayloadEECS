#include <Audio.h>
#include <Wire.h>

// Create an audio test tone, 440 Hz sine wave
AudioSynthWaveformSine sine1;
AudioOutputI2S audioOutput;  // Use AudioOutputI2S2 for secondary I2S pins

// Connect the sine wave output to the I2S output
AudioConnection patchCord1(sine1, 0, audioOutput, 0);
AudioConnection patchCord2(sine1, 0, audioOutput, 1);

// Define the frequencies for the notes
float frequencies[] = {
  0,    // Rest 0
  261,  // C4 1
  277,  // C#4/Db4 2
  293,  // D4 3
  311,  // D#4/Eb4 4
  329,  // E4 5
  349,  // F4 6
  369,  // F#4/Gb4 7
  392,  // G4 8
  415,  // G#4/Ab4 9
  440,  // A4 10
  466,  // A#4/Bb4 11
  493,  // B4 12
  523,  // C5 13
  554,  // C#5/Db5 14
  587,  // D5 15
  622,  // D#5/Eb5 16
  659,  // E5 17
  698,  // F5 18
  740,  // F#5/Gb5 19
  784,  // G5 20
  830,  // G#5/Ab5 21
  880,  // A5 22
  932,  // A#5/Bb5 23
  988,  // B5 24
  1047  // C6 25
};

// Structure to hold both note frequency and duration
struct Note {
  float frequency;
  int duration;
};

// Define the notes and durations for the main theme of Super Mario Bros.
Note marioTheme[] = {
  {frequencies[17], 150}, {frequencies[17], 150}, {frequencies[0], 150}, {frequencies[17], 150}, 
  {frequencies[0], 150}, {frequencies[13], 150}, {frequencies[17], 150}, {frequencies[0], 150},
  {frequencies[20], 150}, {frequencies[0], 500}, {frequencies[8], 150}, {frequencies[0], 500}, 
  {frequencies[13], 150}, {frequencies[0], 300}, {frequencies[8], 150}, {frequencies[0], 150},
  {frequencies[5], 150}, {frequencies[0], 150}, {frequencies[10], 150}, {frequencies[12], 150},
  {frequencies[11], 150}, {frequencies[10], 150}, {frequencies[0], 150}, {frequencies[8], 150}, {frequencies[17], 150},
  {frequencies[0], 150}, {frequencies[20], 150}, {frequencies[0], 150}, {frequencies[22], 150},
  {frequencies[0], 150}, {frequencies[18], 150}, {frequencies[20], 150}, {frequencies[0], 150}, {frequencies[17], 150}, {frequencies[0], 150},
  {frequencies[13], 150}, {frequencies[15], 150}, {frequencies[12], 150}, {frequencies[0], 150},
  {frequencies[12], 150}
};

void setup() {
  // Enable the audio output
  AudioMemory(12);
}

void loop() {
  int speaker_pin = 7;
  // Play the main theme of Super Mario Bros.
  for (int i = 0; i < sizeof(marioTheme) / sizeof(marioTheme[0]); i++) {
    tone(speaker_pin, marioTheme[i].frequency);
    delay(marioTheme[i].duration);  // Use the corresponding duration for each note
    noTone(speaker_pin);  // Stop the tone before the next one starts
  }

  delay(1000);  // Pause before repeating the melody
}
