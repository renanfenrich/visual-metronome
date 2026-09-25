#include "tempo_control.h"

namespace metronome {

TempoControl::TempoControl(uint16_t initialBpm)
    : pot_(initialBpm), capturedAdc_(0), owner_(Owner::POTENTIOMETER_CONTROL) {}

bool TempoControl::acceptPotAdc(uint16_t adc) {
  if (owner_ == Owner::TAP_CONTROL) {
    // Prevent the knob's old position from immediately overriding tap tempo.
    const uint16_t difference = adc > capturedAdc_ ? adc - capturedAdc_ : capturedAdc_ - adc;
    if (difference < kTakeoverThresholdAdc) {
      return false;
    }
    owner_ = Owner::POTENTIOMETER_CONTROL;
  }
  return pot_.acceptAdc(adc);
}

void TempoControl::acceptTap(uint16_t bpm, uint16_t capturedAdc) {
  pot_.setBpm(bpm);
  capturedAdc_ = capturedAdc;
  owner_ = Owner::TAP_CONTROL;
}

uint16_t TempoControl::bpm() const { return pot_.bpm(); }

TempoControl::Owner TempoControl::owner() const { return owner_; }

}  // namespace metronome
