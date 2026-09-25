#include <Arduino.h>

#include "config.h"
#include "debouncer.h"
#include "pattern.h"
#include "tap_tempo.h"
#include "tempo.h"
#include "tempo_control.h"
#include "time_signature.h"
#include "transport.h"

namespace {

bool sanityActive = false;
unsigned long sanityStartedAt = 0;
constexpr unsigned long kSanityDurationMs = 150;
constexpr unsigned long kBpmSampleIntervalMs = 25;
metronome::Tempo tempo;
metronome::TimeSignature timeSignature;
metronome::Transport transport(tempo, timeSignature.pattern());
metronome::TempoControl tempoControl(tempo.bpm());
metronome::Debouncer startStopButton;
metronome::Debouncer tapButton;
metronome::Debouncer modeButton;
metronome::TapTempo tapTempo;
unsigned long lastBpmSampleAt = 0;
uint16_t lastAdc = 0;

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

void setBpm(uint16_t bpm, bool fromTap, uint32_t nowUs) {
  tempo.setBpm(bpm);
  transport.setTempo(tempo, nowUs);
  Serial.print(F("BPM: "));
  Serial.print(tempo.bpm());
  Serial.println(fromTap ? F(" (tap)") : F(" (pot)"));
}
}  // namespace

void setup() {
  Serial.begin(9600);

  for (uint8_t i = 0; i < metronome::kLedCount; ++i) {
    pinMode(metronome::kLedPins[i], OUTPUT);
  }
  pinMode(metronome::kStartStopPin, INPUT_PULLUP);
  pinMode(metronome::kTapTempoPin, INPUT_PULLUP);
  pinMode(metronome::kModePin, INPUT_PULLUP);
  setLeds(LOW);
  lastAdc = analogRead(metronome::kBpmPin);

  Serial.println(F("Visual Metronome"));
  Serial.println(F("Firmware: 0.1.0"));
  Serial.println(F("Goal: G5"));
  Serial.println(F("Status: READY"));

  setLeds(HIGH);
  sanityStartedAt = millis();
  sanityActive = true;
}

void loop() {
  const unsigned long nowMs = millis();
  const unsigned long nowUs = micros();
  if (nowMs - lastBpmSampleAt >= kBpmSampleIntervalMs) {
    lastBpmSampleAt = nowMs;
    lastAdc = analogRead(metronome::kBpmPin);
    if (tempoControl.acceptPotAdc(lastAdc)) {
      setBpm(tempoControl.bpm(), false, nowUs);
    }
  }

  if (sanityActive && millis() - sanityStartedAt >= kSanityDurationMs) {
    setLeds(LOW);
    sanityActive = false;
    transport.toggle(nowUs);
  }

  const bool startPressed = startStopButton.update(
      digitalRead(metronome::kStartStopPin) == HIGH, nowMs);
  const bool tapPressed = tapButton.update(
      digitalRead(metronome::kTapTempoPin) == HIGH, nowMs);
  const bool modePressed = modeButton.update(
      digitalRead(metronome::kModePin) == HIGH, nowMs);

  if (!sanityActive && startPressed) {
    if (transport.toggle(nowUs)) {
      Serial.println(F("Transport: RUNNING"));
    } else {
      setLeds(LOW);
      Serial.println(F("Transport: STOPPED"));
    }
  }

  if (!sanityActive && tapPressed && tapTempo.tap(nowUs)) {
    tempoControl.acceptTap(tapTempo.bpm(), lastAdc);
    setBpm(tempoControl.bpm(), true, nowUs);
  }

  if (!sanityActive && modePressed) {
    timeSignature.cycle();
    transport.setPattern(timeSignature.pattern());
    setLeds(LOW);
    Serial.print(F("Signature: "));
    Serial.println(timeSignature.label());
  }

  if (transport.update(nowUs) > 0) {
    showBeat(transport.sequence().position());
  }
}
