#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#define SEC_IN_MICROSEC 1000000
#define A4 440.0
#define As4 466.16
#define B4 493.88
#define C4 261.63
#define Cs4 277.18
#define D4 293.66
#define Ds4 311.13
#define E4 329.63
#define F4 349.23
#define Fs4 369.99
#define G4 392.0
#define Gs4 415.3

const int speakerPin = 9;
enum WaveType { SQUARE, SINUSOID, PHASOR, ORGAN };

// 32-sample Sine Wave for the Sinusoid option
const uint8_t sineTable[] PROGMEM = {
  127, 151, 175, 197, 216, 232, 244, 251, 254, 251, 244, 232, 216, 197, 175, 151, 
  127, 102, 78, 56, 37, 21, 9, 2, 0, 2, 9, 21, 37, 56, 78, 102
};

const uint8_t organTable[] PROGMEM = {
  127, 180, 210, 215, 200, 185, 180, 190, 200, 205, 190, 160, 127, 94, 64, 49, 
  54, 64, 74, 69, 54, 39, 44, 74, 127
};

void initAudio() {
  pinMode(speakerPin, OUTPUT);
  TCCR1A = _BV(COM1A1) | _BV(WGM10); // Fast PWM
  TCCR1B = _BV(WGM12) | _BV(CS10); // 62.5kHz carrier
}

void play_note(double frequency, int length, WaveType wave) {
  const int steps = 32;
  // Calculate delay per step in microseconds
  uint32_t step_delay = (1000000UL / (uint32_t)(frequency * steps));
  uint32_t duration_limit = (uint32_t)length * 125000UL; // 1/8th second units
  uint32_t start_audio = micros();

  while (micros() - start_audio <= duration_limit) {
    for (int i = 0; i < steps; i++) {
      if (wave == SQUARE) {
        OCR1A = (i < steps / 2) ? 255 : 0;
      } 
      else if (wave == PHASOR) {
        OCR1A = (i * 255) / steps;
      } 
      else if (wave == SINUSOID) {
        OCR1A = pgm_read_byte(&sineTable[i]);
      }
      else if (wave == ORGAN) {
        OCR1A = pgm_read_byte(&organTable[i]);
      }
      delayMicroseconds(step_delay);
    }
  }
  OCR1A = 0; // Turn off the sound
}

#endif