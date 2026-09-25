#include <Arduino.h>

#include "beat_sequence.h"
#include "clock.h"
#include "config.h"
#include "pattern.h"
#include "tempo.h"

namespace {

bool sanityActive = false;
unsigned long sanityStartedAt = 0;
constexpr unsigned long kSanityDurationMs = 150;
metronome::Tempo tempo;
metronome::Clock clock(tempo);
metronome::BeatSequence sequence(metronome::Pattern(4, 0));

void setLeds(uint8_t state) {
  for (uint8_t i = 0; i < metronome::kLedCount; ++i) {
    digitalWrite(metronome::kLedPins[i], state);
  }
}

void showBeat(uint8_t position) {
  for (uint8_t i = 0; i < metronome::kLedCount; ++i) {
    digitalWrite(metronome::kLedPins[i], i == position ? HIGH : LOW);
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
  Serial.println(F("Goal: G2"));
  Serial.println(F("Status: READY"));

  setLeds(HIGH);
  sanityStartedAt = millis();
  sanityActive = true;
}

void loop() {
  if (sanityActive && millis() - sanityStartedAt >= kSanityDurationMs) {
    setLeds(LOW);
    sanityActive = false;
    clock.start(micros());
  }

  const uint16_t elapsedBeats = clock.update(micros());
  if (elapsedBeats > 0) {
    sequence.advance(elapsedBeats);
    showBeat(sequence.position());
  }
}
