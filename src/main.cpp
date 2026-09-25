#include <Arduino.h>

#include "config.h"

namespace {

bool sanityActive = false;
unsigned long sanityStartedAt = 0;
constexpr unsigned long kSanityDurationMs = 150;

void setLeds(uint8_t state) {
  for (uint8_t i = 0; i < metronome::kLedCount; ++i) {
    digitalWrite(metronome::kLedPins[i], state);
  }
}
}  // namespace

void setup() {
  Serial.begin(9600);

  for (uint8_t i = 0; i < metronome::kLedCount; ++i) {
    pinMode(metronome::kLedPins[i], OUTPUT);
  }
  setLeds(LOW);

  Serial.println(F("Visual Metronome"));
  Serial.println(F("Firmware: 0.1.0"));
  Serial.println(F("Goal: G0"));
  Serial.println(F("Status: READY"));

  setLeds(HIGH);
  sanityStartedAt = millis();
  sanityActive = true;
}

void loop() {
  if (sanityActive && millis() - sanityStartedAt >= kSanityDurationMs) {
    setLeds(LOW);
    sanityActive = false;
  }
}
